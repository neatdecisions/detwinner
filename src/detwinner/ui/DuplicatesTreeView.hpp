/*
 ===============================================================================
 Name        : DuplicatesTreeView.hpp
 Author      : NeatDecisions
 Version     :
 Copyright   : Copyright © 2018–2023 Neat Decisions. All rights reserved.
 Description : Detwinner
 ===============================================================================
 */

#pragma once

#include <gtkmm.h>
#include <gtkmm/treestore.h>

#include <callbacks/IDeferredAction.hpp>
#include <callbacks/IDuplicateReceiver.hpp>
#include <logic/CommonDataTypes.hpp>
#include <tools/AbstractFileDeleter.hpp>

namespace detwinner::ui {

class DuplicatesTreeView : public Gtk::TreeView
{
public:
	enum class Mode { Normal, Images };

	struct DuplicateStats
	{
		unsigned int groupCount = 0;
		unsigned int fileCount = 0;
		unsigned long long totalSize = 0;
		unsigned long long wastedSize = 0;
	};

	DuplicatesTreeView();
	virtual ~DuplicatesTreeView() noexcept override;

	void setMode(Mode mode);
	bool empty() const;
	void clear();
	bool atLeastOneTopLevelItemChecked() const;
	DuplicateStats calculateStats() const;

	// sorting methods
	void sortByTotalSize(bool asc);
	void sortBySingleFileSize(bool asc);
	void sortByNumberOfFiles(bool asc);

	// smart selection methods
	callbacks::IDeferredAction::Ptr smartSelect_KeepFirstInGroup();
	callbacks::IDeferredAction::Ptr smartSelect_KeepLastInGroup();

	callbacks::IDeferredAction::Ptr smartSelect_KeepEarliestModified();
	callbacks::IDeferredAction::Ptr smartSelect_KeepLatestModified();

	callbacks::IDeferredAction::Ptr smartSelect_KeepShortestName();
	callbacks::IDeferredAction::Ptr smartSelect_KeepLongestName();

	callbacks::IDeferredAction::Ptr smartSelect_KeepShortestPath();
	callbacks::IDeferredAction::Ptr smartSelect_KeepLongestPath();

	callbacks::IDeferredAction::Ptr smartSelect_KeepLowestResolution();
	callbacks::IDeferredAction::Ptr smartSelect_KeepHighestResolution();

	// bulk selection methods
	callbacks::IDeferredAction::Ptr selectAll();
	callbacks::IDeferredAction::Ptr clearSelection();
	callbacks::IDeferredAction::Ptr invertSelection();

	// deletion methods
	callbacks::IDeferredAction::Ptr deletePermanently();
	callbacks::IDeferredAction::Ptr deleteToTrash();
	callbacks::IDeferredAction::Ptr deleteToBackupFolder(const std::string & folder, Gtk::Window * dialogParent);

	callbacks::IDuplicateReceiver::Ptr createPopulationDelegate();

	// row selection signal
	using signal_duplicate_selected = sigc::signal<void, Glib::ustring, Glib::ustring>;
	signal_duplicate_selected on_duplicate_selected();

	using signal_stats_changed = sigc::signal<void>;
	signal_stats_changed on_stats_changed();

protected:
	// row selection handler
	virtual void on_cursor_changed() override;
	virtual void on_row_activated(const Gtk::TreeModel::Path & path, Gtk::TreeViewColumn * treeColumn) override;
	virtual void on_size_allocate(Gtk::Allocation & s) override;
	virtual void on_hide() override;
	virtual bool on_button_press_event(GdkEventButton * button_event) override;

private:
	enum class CheckState { Checked, Unchecked, Mixed };

	struct FileModelColumn : public Gtk::TreeModel::ColumnRecord
	{
		Gtk::TreeModelColumn<bool> locked;
		Gtk::TreeModelColumn<CheckState> checkState;
		Gtk::TreeModelColumn<Glib::ustring> filePath;
		Gtk::TreeModelColumn<unsigned long long> fileSize;
		Gtk::TreeModelColumn<Glib::DateTime> updateTime;
		Gtk::TreeModelColumn<unsigned int> width;
		Gtk::TreeModelColumn<unsigned int> height;
		FileModelColumn()
		{
			add(checkState);
			add(locked);
			add(filePath);
			add(fileSize);
			add(updateTime);
			add(width);
			add(height);
		}
	};

	// checkbox handler
	virtual void on_cell_toggled(const Glib::ustring & path);

	// check getter/setter
	CheckState getCheck(const Gtk::TreeIter & iter) const;
	bool setCheck(const Gtk::TreeIter & iter, CheckState checkState, bool noUp, bool noDown);

	// cell rendering handles
	void on_render_filename(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_path(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_lock(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_size(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_resolution(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_update_time(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_toggle(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);
	void on_render_icon(Gtk::CellRenderer * cellRenderer, const Gtk::TreeModel::iterator & iter);

	// tree sorting functions
	int sort_func_total_size(const Gtk::TreeModel::iterator &, const Gtk::TreeModel::iterator &) const;
	int sort_func_single_file_size(const Gtk::TreeModel::iterator &, const Gtk::TreeModel::iterator &) const;
	int sort_func_number_of_files(const Gtk::TreeModel::iterator &, const Gtk::TreeModel::iterator &) const;

	// context menu handling functions
	void on_duplicate_file_open() const;
	void on_duplicate_file_open_folder() const;
	void on_group_smart_select_keep_first();
	void on_group_smart_select_keep_last();
	void on_group_smart_select_keep_earliest();
	void on_group_smart_select_keep_latest();
	void on_group_smart_select_keep_shortestname();
	void on_group_smart_select_keep_longestname();
	void on_group_smart_select_keep_shortestpath();
	void on_group_smart_select_keep_longestpath();
	void on_group_smart_select_keep_lowestresolution();
	void on_group_smart_select_keep_highestresolution();
	void on_group_select_all();
	void on_group_clear_selection();
	void on_group_invert_selection();
	void on_group_exclude();

	Glib::ustring getPreviewPath(const Gtk::TreeIter & iter) const;
	Glib::ustring extractFullPath(const Gtk::TreeIter & iter) const;
	Glib::DateTime getUpdateTime(const Gtk::TreeIter & iter);
	bool isDuplicateFileIter(const Gtk::TreeIter & iter) const;
	Gtk::TreeIter getSelectedIter() const;

	class PopulationDelegate : public callbacks::IDuplicateReceiver
	{
	public:
		explicit PopulationDelegate(DuplicatesTreeView & tree);
		virtual callbacks::IDeferredAction::Ptr populate(logic::DuplicatesList && container) override;

	private:
		DuplicatesTreeView & m_tree;
	};

	class TreeAction : public callbacks::IDeferredAction
	{
	public:
		explicit TreeAction(DuplicatesTreeView & tree);
		virtual double getProgress() const override;
		virtual Result getStatus() const override;

	protected:
		DuplicatesTreeView & m_tree;
		std::size_t m_totalItems;
		std::size_t m_currentItem;
		Gtk::TreeModel::iterator m_iter;
		Result m_resultStatus;
	};

	struct ISmartSelector
	{
		using Ptr = std::shared_ptr<ISmartSelector>;
		virtual ~ISmartSelector() = default;
		virtual void select(DuplicatesTreeView & tree, Gtk::TreeIter iter) = 0;
	};

	class SmartSelector : public ISmartSelector
	{
	public:
		using SelectFunc = std::function<bool(const Gtk::TreeModel::iterator &, const Gtk::TreeModel::iterator &)>;
		using IgnoreFunc = std::function<bool(const Gtk::TreeModel::iterator &)>;
		explicit SmartSelector(
				const SelectFunc & selectFunc,
				const IgnoreFunc & ignoreFunc = [](const Gtk::TreeModel::iterator &) { return false; });
		virtual void select(DuplicatesTreeView & tree, Gtk::TreeIter iter) override;

	private:
		SelectFunc m_selectFunc;
		IgnoreFunc m_ignoreFunc;
	};

	class TreeSelect_Smart : public TreeAction
	{
	public:
		TreeSelect_Smart(const ISmartSelector::Ptr & smartSelector, DuplicatesTreeView & tree);
		virtual bool processNext() override;

	private:
		ISmartSelector::Ptr m_smartSelector;
	};

	class TreeSelect_Bulk : public TreeAction
	{
	public:
		using SelectFunc = std::function<CheckState(const Gtk::TreeModel::iterator &)>;
		TreeSelect_Bulk(const SelectFunc & selectFunc, DuplicatesTreeView & tree);
		virtual bool processNext() override;

	private:
		SelectFunc m_selectFunc;
	};

	class TreePopulateAction : public callbacks::IDeferredAction
	{
	public:
		explicit TreePopulateAction(DuplicatesTreeView & tree, logic::DuplicatesList && values);
		virtual ~TreePopulateAction() noexcept override;
		virtual double getProgress() const override;
		virtual bool processNext() override;
		virtual Result getStatus() const override;

	private:
		void beginBatch();
		void endBatch();
		DuplicatesTreeView & m_tree;
		logic::DuplicatesList m_values;
		std::size_t m_currentItem;
		bool m_batchStarted;
	};

	class TreeDeleteAction : public TreeAction
	{
	public:
		explicit TreeDeleteAction(DuplicatesTreeView & tree, tools::AbstractFileDeleter::Ptr fileDeleter);
		virtual bool processNext() override;

	private:
		void updateResultStatus(Result stepStatus);
		tools::AbstractFileDeleter::Ptr m_fileDeleter;
	};

	ISmartSelector::Ptr createSmartSelector_KeepFirstInGroup();
	ISmartSelector::Ptr createSmartSelector_KeepLastInGroup();
	ISmartSelector::Ptr createSmartSelector_KeepEarliestModified();
	ISmartSelector::Ptr createSmartSelector_KeepLatestModified();
	ISmartSelector::Ptr createSmartSelector_KeepShortestName();
	ISmartSelector::Ptr createSmartSelector_KeepLongestName();
	ISmartSelector::Ptr createSmartSelector_KeepShortestPath();
	ISmartSelector::Ptr createSmartSelector_KeepLongestPath();
	ISmartSelector::Ptr createSmartSelector_KeepLowestResolution();
	ISmartSelector::Ptr createSmartSelector_KeepHighestResolution();

	std::unique_ptr<Gtk::Menu> loadMenu(const Glib::ustring & name);

	Gtk::CellRendererToggle m_toggleRenderer;
	Gtk::CellRendererPixbuf m_lockRenderer;
	Gtk::CellRendererText m_sizeRenderer;
	Gtk::CellRendererText m_resolutionRenderer;
	Gtk::CellRendererText m_fileNameRenderer;
	Gtk::CellRendererText m_pathRenderer;
	Gtk::CellRendererText m_updateTimeRenderer;
	Gtk::CellRendererPixbuf m_iconRenderer;

	FileModelColumn m_columns;
	Glib::RefPtr<Gtk::TreeStore> m_store;
	Glib::RefPtr<Gtk::Builder> m_refBuilder;
	std::unique_ptr<Gtk::Menu> m_refDuplicateFileMenu;
	std::unique_ptr<Gtk::Menu> m_refDuplicateGroupMenu;
	Glib::RefPtr<Gio::SimpleActionGroup> m_refActionGroup;

	signal_duplicate_selected m_signalDuplicateSelected;
	signal_stats_changed m_signalStatsChanged;

	Mode m_mode;
	bool m_adapted;

	Gtk::TreeViewColumn * m_columnResolution;
};

} // namespace detwinner::ui
