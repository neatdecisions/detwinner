/*
 ===============================================================================
 Name        : PlacesSidebar.cpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#include <ui/PlacesSidebar.hpp>

namespace detwinner::ui {

namespace {

//==============================================================================
// PlacesRow
//==============================================================================
class PlacesRow : public Gtk::ListBoxRow
{
public:
	enum class Kind { Special, Volume, Bookmark };

	PlacesRow(Kind kind,
	          const Glib::ustring & text,
	          const Glib::RefPtr<Gio::Icon> & icon,
	          const Glib::RefPtr<Gio::File> & location,
	          const Glib::RefPtr<Gio::Drive> & drive,
	          const Glib::RefPtr<Gio::Volume> & volume,
	          const Glib::RefPtr<Gio::Mount> & mount)
			: m_kind(kind), m_icon(icon, Gtk::ICON_SIZE_MENU), m_label(text), m_location(location), m_drive(drive),
				m_volume(volume), m_mount(mount)
	{
		set_margin_bottom(1);
		set_margin_top(1);
		m_icon.get_style_context()->add_class("sidebar-icon");
		m_label.get_style_context()->add_class("sidebar-label");
		get_style_context()->add_class("sidebar-row");

		m_label.set_justify(Gtk::Justification::JUSTIFY_LEFT);
		m_label.set_ellipsize(Pango::EllipsizeMode::ELLIPSIZE_END);

		m_ejectButton.set_image_from_icon_name("media-eject-symbolic", Gtk::ICON_SIZE_MENU);
		m_ejectButton.set_no_show_all(true);
		m_ejectButton.set_margin_start(4);
		m_ejectButton.set_halign(Gtk::Align::ALIGN_CENTER);
		m_ejectButton.set_valign(Gtk::Align::ALIGN_CENTER);
		m_ejectButton.get_style_context()->add_class("image-button");
		m_ejectButton.get_style_context()->add_class("sidebar-button");

		Gtk::Box * pBox = Gtk::make_managed<Gtk::Box>(Gtk::ORIENTATION_HORIZONTAL);
		pBox->pack_start(m_icon, false, false);
		pBox->pack_start(m_label, false, true);
		pBox->pack_end(m_ejectButton, false, true);
		pBox->set_spacing(10);
		add(*pBox);

		show_all_children();
		updateEjectButton();
	}

	Glib::RefPtr<Gio::File> getLocation() const { return m_mount ? m_mount->get_default_location() : m_location; }

	Kind getKind() const { return m_kind; }

	Glib::RefPtr<Gio::Volume> getVolume() const { return m_volume; }

	void updateEjectButton()
	{
		constexpr auto isMountUnmountable = [](const Glib::RefPtr<Gio::Mount> & mount) {
			return mount && (mount->can_eject() || mount->can_unmount());
		};

		m_ejectButton.set_visible((m_drive && m_drive->can_eject()) ||
		                          (m_volume && (m_volume->can_eject() || isMountUnmountable(m_volume->get_mount()))) ||
		                          isMountUnmountable(m_mount));
		m_ejectButton.signal_clicked().connect(sigc::mem_fun(*this, &PlacesRow::onEjectButtonClicked));
	}

	static void AddToListBox(PlacesRow::Kind kind,
	                         const Glib::ustring & name,
	                         const Glib::RefPtr<Gio::Icon> & icon,
	                         const Glib::ustring & path,
	                         Gtk::ListBox & listBox,
	                         const Glib::RefPtr<Gio::Drive> & drive = {},
	                         const Glib::RefPtr<Gio::Volume> & volume = {},
	                         const Glib::RefPtr<Gio::Mount> & mount = {})
	{
		auto location = Gio::File::create_for_path(path);
		auto row = Gtk::make_managed<PlacesRow>(kind, name, icon, location, drive, volume, mount);
		listBox.append(*row);
		row->show();
	}

private:
	void onEjectButtonClicked()
	{
		if (m_mount)
		{
			if (m_mount->can_eject())
			{
				m_mount->eject(sigc::mem_fun(*this, &PlacesRow::onEjectFinished));
			} else if (m_mount->can_unmount())
			{
				m_mount->unmount(sigc::mem_fun(*this, &PlacesRow::onUnmountFinished));
			}
		}
	}

	void onEjectFinished(Glib::RefPtr<Gio::AsyncResult> & asyncResult)
	{
		if (m_mount && m_mount->eject_finish(asyncResult)) updateEjectButton();
	}

	void onUnmountFinished(Glib::RefPtr<Gio::AsyncResult> & asyncResult)
	{
		if (m_mount && m_mount->unmount_finish(asyncResult)) updateEjectButton();
	}

	Kind m_kind;
	Gtk::Image m_icon;
	Gtk::Label m_label;
	Gtk::Button m_ejectButton;
	Glib::RefPtr<Gtk::SizeGroup> m_sizeGroup;
	const Glib::RefPtr<Gio::File> m_location;
	const Glib::RefPtr<Gio::Drive> m_drive;
	const Glib::RefPtr<Gio::Volume> m_volume;
	const Glib::RefPtr<Gio::Mount> m_mount;
};

} // namespace

//==============================================================================
// PlacesSidebar
//==============================================================================

//------------------------------------------------------------------------------
PlacesSidebar::PlacesSidebar() : m_volumeMonitor(Gio::VolumeMonitor::get())
{
	set_policy(Gtk::PolicyType::POLICY_NEVER, Gtk::PolicyType::POLICY_AUTOMATIC);
	set_shadow_type(Gtk::ShadowType::SHADOW_IN);
	get_style_context()->add_class("sidebar");
	get_style_context()->set_junction_sides(Gtk::JUNCTION_RIGHT | Gtk::JUNCTION_RIGHT);
	set_size_request(140, 280);

	m_listBox.get_style_context()->add_class("navigation-sidebar");
	m_listBox.set_activate_on_single_click(true);
	m_listBox.set_selection_mode(Gtk::SELECTION_BROWSE);
	m_listBox.signal_row_activated().connect(sigc::mem_fun(*this, &PlacesSidebar::onRowActivated));
	m_listBox.set_header_func(sigc::mem_fun(*this, &PlacesSidebar::onDrawListBoxHeader));

	add(m_listBox);
	update();

	m_volumeMonitor->signal_mount_removed().connect([this](const Glib::RefPtr<Gio::Mount> &) { update(); });
	m_volumeMonitor->signal_mount_added().connect([this](const Glib::RefPtr<Gio::Mount> &) { update(); });
	m_volumeMonitor->signal_mount_changed().connect([this](const Glib::RefPtr<Gio::Mount> &) { update(); });
	m_volumeMonitor->signal_volume_removed().connect([this](const Glib::RefPtr<Gio::Volume> &) { update(); });
	m_volumeMonitor->signal_volume_added().connect([this](const Glib::RefPtr<Gio::Volume> &) { update(); });
	m_volumeMonitor->signal_volume_changed().connect([this](const Glib::RefPtr<Gio::Volume> &) { update(); });
	m_volumeMonitor->signal_drive_disconnected().connect([this](const Glib::RefPtr<Gio::Drive> &) { update(); });
	m_volumeMonitor->signal_drive_connected().connect([this](const Glib::RefPtr<Gio::Drive> &) { update(); });
	m_volumeMonitor->signal_drive_changed().connect([this](const Glib::RefPtr<Gio::Drive> &) { update(); });
}

//------------------------------------------------------------------------------
void
PlacesSidebar::onDrawListBoxHeader(Gtk::ListBoxRow * row, Gtk::ListBoxRow * before)
{
	row->unset_header();
	if (!before)
	{
		row->set_margin_top(4);
	}
	if (before && dynamic_cast<PlacesRow *>(before)->getKind() != dynamic_cast<PlacesRow *>(row)->getKind())
	{
		auto separator = Gtk::make_managed<Gtk::Separator>(Gtk::ORIENTATION_HORIZONTAL);
		separator->set_margin_top(4);
		separator->set_margin_bottom(4);
		row->set_header(*separator);
	}
}

//------------------------------------------------------------------------------
void
PlacesSidebar::setLocation(const Glib::RefPtr<const Gio::File> & location)
{
	m_listBox.unselect_all();
	m_currentLocation = location;
	if (m_currentLocation)
	{
		for (int i = 0; auto row = dynamic_cast<PlacesRow *>(m_listBox.get_row_at_index(i)); ++i)
		{
			if (location->equal(row->getLocation()))
			{
				m_listBox.select_row(*row);
				break;
			}
		}
	}
	m_signalOpenLocation.emit(m_currentLocation);
}

//------------------------------------------------------------------------------
void
PlacesSidebar::update()
{
	m_listBox.foreach ([this](Widget & w) { m_listBox.remove(w); });
	PlacesRow::AddToListBox(PlacesRow::Kind::Special, "Home", Gio::ThemedIcon::create("user-home-symbolic", true),
	                        Glib::get_home_dir(), m_listBox);
	static const auto specialDirs = std::array<std::pair<Glib::UserDirectory, Glib::ustring>, 8>{
			std::pair<Glib::UserDirectory, Glib::ustring>{Glib::USER_DIRECTORY_DESKTOP, "user-desktop-symbolic"},
			std::pair<Glib::UserDirectory, Glib::ustring>{Glib::USER_DIRECTORY_DOCUMENTS, "folder-documents-symbolic"},
			std::pair<Glib::UserDirectory, Glib::ustring>{Glib::USER_DIRECTORY_DOWNLOAD, "folder-download-symbolic"},
			std::pair<Glib::UserDirectory, Glib::ustring>{Glib::USER_DIRECTORY_MUSIC, "folder-music-symbolic"},
			std::pair<Glib::UserDirectory, Glib::ustring>{Glib::USER_DIRECTORY_PICTURES, "folder-pictures-symbolic"},
			std::pair<Glib::UserDirectory, Glib::ustring>{Glib::USER_DIRECTORY_PUBLIC_SHARE, "folder-publicshare-symbolic"},
			std::pair<Glib::UserDirectory, Glib::ustring>{Glib::USER_DIRECTORY_TEMPLATES, "folder-templates-symbolic"},
			std::pair<Glib::UserDirectory, Glib::ustring>{Glib::USER_DIRECTORY_VIDEOS, "folder-videos-symbolic"}};
	for (const auto & dir : specialDirs)
	{
		auto location = Gio::File::create_for_path(Glib::get_user_special_dir(dir.first));
		PlacesRow::AddToListBox(PlacesRow::Kind::Special, location->get_basename(),
		                        Gio::ThemedIcon::create(dir.second, true), location->get_path(), m_listBox);
	}

	PlacesRow::AddToListBox(PlacesRow::Kind::Volume, Glib::get_host_name(),
	                        Gio::ThemedIcon::create("drive-harddisk-symbolic", true), "/", m_listBox);

	for (const auto & drive : m_volumeMonitor->get_connected_drives())
	{
		if (drive->get_volumes().empty() && drive->is_media_removable() && !drive->is_media_check_automatic())
		{
			PlacesRow::AddToListBox(PlacesRow::Kind::Volume, drive->get_name(), drive->get_icon(), "", m_listBox, drive);
		} else
		{
			for (const auto & volume : drive->get_volumes())
			{
				if (volume->get_identifier(G_VOLUME_IDENTIFIER_KIND_CLASS) == "network") continue;
				const auto & mount = volume->get_mount();
				if (mount)
				{
					PlacesRow::AddToListBox(PlacesRow::Kind::Volume, mount->get_name(), mount->get_symbolic_icon(),
					                        mount->get_default_location()->get_path(), m_listBox, drive, volume, mount);
				} else
				{
					PlacesRow::AddToListBox(PlacesRow::Kind::Volume, volume->get_name(), volume->get_symbolic_icon(), {},
					                        m_listBox, drive, volume);
				}
			}
		}
	}

	for (const auto & volume : m_volumeMonitor->get_volumes())
	{
		if (volume->get_drive()) continue;
		if (volume->get_identifier(G_VOLUME_IDENTIFIER_KIND_CLASS) == "network") continue;
		const auto & mount = volume->get_mount();
		if (mount)
		{
			PlacesRow::AddToListBox(PlacesRow::Kind::Volume, mount->get_name(), mount->get_symbolic_icon(),
			                        mount->get_default_location()->get_path(), m_listBox, {}, volume, mount);
		} else
		{
			PlacesRow::AddToListBox(PlacesRow::Kind::Volume, volume->get_name(), volume->get_symbolic_icon(), {}, m_listBox,
			                        {}, volume);
		}
	}

	for (const auto & mount : m_volumeMonitor->get_mounts())
	{
		if (mount->get_volume()) continue;
		if (mount->is_shadowed()) continue;
		const auto mountLocation = mount->get_default_location();
		if (!mountLocation->is_native()) continue;
		PlacesRow::AddToListBox(PlacesRow::Kind::Volume, mount->get_name(), mount->get_symbolic_icon(),
		                        mountLocation->get_path(), m_listBox, {}, {}, mount);
	}

	auto filename = Glib::build_filename(Glib::get_user_config_dir(), "gtk-3.0", "bookmarks");
	if (!Glib::file_test(filename, Glib::FILE_TEST_EXISTS))
	{
		filename = Glib::build_filename(Glib::get_home_dir(), ".gtk-bookmarks");
	}

	try
	{
		const auto fileContents = Glib::file_get_contents(filename);
		const auto bookmarks = Glib::Regex::split_simple("\\R", fileContents);
		for (const auto & bookmark : bookmarks)
		{
			auto spacePos = bookmark.find(' ');
			auto gioFile = Gio::File::create_for_uri(bookmark.substr(0, spacePos));

			if (!gioFile->query_exists() || std::any_of(specialDirs.begin(), specialDirs.end(), [&gioFile](const auto & dir) {
						return Gio::File::create_for_path(Glib::get_user_special_dir(dir.first))->equal(gioFile);
					}))
			{
				continue;
			}

			const auto fileInfo = gioFile->query_info("standard::display-name,standard::symbolic-icon");
			auto label =
					spacePos < bookmark.size() ? bookmark.substr(spacePos + 1) : Glib::ustring(fileInfo->get_display_name());
			PlacesRow::AddToListBox(PlacesRow::Kind::Bookmark, label, fileInfo->get_symbolic_icon(), gioFile->get_path(),
			                        m_listBox);
		}
	} catch (...)
	{
		g_warning("Error while retrieving the bookmarks");
	}

	if (m_currentLocation && m_currentLocation->query_exists())
	{
		setLocation(m_currentLocation);
	} else
	{
		setLocation(Gio::File::create_for_path(Glib::get_home_dir()));
	}
}

//------------------------------------------------------------------------------
void
PlacesSidebar::onRowActivated(Gtk::ListBoxRow * row)
{
	m_listBox.select_row(*row);
	auto * placesRow = dynamic_cast<PlacesRow *>(row);

	if (placesRow->getVolume())
	{
		auto mount = placesRow->getVolume()->get_mount();
		if (mount)
		{
			setLocation(mount->get_default_location());
		} else if (!m_volumeBeingMounted)
		{
			auto mountOperation = Gio::MountOperation::create();
			mountOperation->set_password_save(Gio::PasswordSave::PASSWORD_SAVE_FOR_SESSION);
			m_volumeBeingMounted = placesRow->getVolume();
			placesRow->getVolume()->mount(mountOperation, sigc::mem_fun(*this, &PlacesSidebar::onMountFinished));
		}
	} else
	{
		setLocation(placesRow->getLocation());
	}
}

//------------------------------------------------------------------------------
void
PlacesSidebar::onMountFinished(Glib::RefPtr<Gio::AsyncResult> & asyncResult)
{
	if (!m_volumeBeingMounted) return;
	if (m_volumeBeingMounted->mount_finish(asyncResult))
	{
		if (auto mount = m_volumeBeingMounted->get_mount()) setLocation(mount->get_default_location());
	}
	m_volumeBeingMounted.reset();
}

} // namespace detwinner::ui
