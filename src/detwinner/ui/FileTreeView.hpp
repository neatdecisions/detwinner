/*
 ===============================================================================
 Name        : FileTreeView.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#ifndef UI_FILETREEVIEW_HPP_
#define UI_FILETREEVIEW_HPP_

#include <gtkmm/treeview.h>
#include <gtkmm/treestore.h>
#include <gtkmm/cellrendererpixbuf.h>
#include <giomm/file.h>


namespace detwinner {
namespace ui {


class FileTreeView : public Gtk::TreeView
{
public:
	FileTreeView();
	std::vector<std::string> fetchSelectedPaths() const;
	void load(const std::string & path);
	void selectAll();
	void clearSelection();
	void refresh();
	void set_show_hidden(bool value);

protected:
	bool on_test_expand_row(const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path) override;
	void on_row_expanded(const Gtk::TreeModel::iterator & iter, const Gtk::TreeModel::Path & path) override;
	void on_cell_toggled(const Glib::ustring & path);

	void on_render_filename(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_toggle(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_icon(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);

private:
	enum class CheckState_t
	{
		Checked,
		Unchecked,
		Mixed
	};

	struct FileModelColumn : public Gtk::TreeModel::ColumnRecord
	{
		Gtk::TreeModelColumn<CheckState_t> checkState;
		Gtk::TreeModelColumn<Glib::ustring> fullPath;
		Gtk::TreeModelColumn<bool> fake;
		FileModelColumn() { add(checkState); add(fake); add(fullPath); }
	};

	using FolderTreeItem = Glib::RefPtr<Gio::FileInfo>;

	void fetchSelectedPathsInternal(const Gtk::TreeRow & treeRow, std::vector<std::string> & paths) const;

	void fillTreeRow(
			const Glib::RefPtr<Gio::FileInfo> & iFileInfo,
			const std::string & iPath,
			Gtk::TreeRow & ioTreeRow);

	void collectChilden(const std::string & parentPath, std::vector<FolderTreeItem> & childen);

	CheckState_t getCheck(const Gtk::TreeIter & iter) const;
	bool setCheck(const Gtk::TreeIter & iter, CheckState_t checkState, bool noUp, bool noDown);

	void refresh_children(const Gtk::TreeModel::iterator & iter);
	void refresh_internal(const Gtk::TreeModel::iterator & iter);
	void reload_from_filesystem(const Gtk::TreeModel::iterator & iter);
	std::string getFileName(const Gtk::TreeModel::iterator & iter);

	template<class TreeIter_t>
	void fillChildren(const std::string & path, CheckState_t checkState, TreeIter_t & treeRow);

	FileModelColumn m_columns;
	Glib::RefPtr<Gtk::TreeStore> m_store;

	std::string m_basePath;
	bool m_showHiddenFiles = false;
};


}}

#endif /* UI_FILETREEVIEW_HPP_ */
