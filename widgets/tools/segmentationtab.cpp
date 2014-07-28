#include "segmentationtab.h"

#include "knossos-global.h"

#include <QApplication>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QString>

#include <chrono>

extern stateInfo *state;

void TouchedObjectModel::recreate() {
    beginResetModel();
    objectCache.clear();
    for (auto & obj : Segmentation::singleton().touchedObjects()) {
        objectCache.emplace_back(obj);
    }
    endResetModel();
}

int SegmentationObjectModel::rowCount(const QModelIndex &) const {
    return objectCache.size();
}

int SegmentationObjectModel::columnCount(const QModelIndex &) const {
    return header.size();
}

QVariant SegmentationObjectModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return header[section];
    } else {
        return QVariant();//return invalid QVariant
    }
}

QVariant SegmentationObjectModel::data(const QModelIndex & index, int role) const {
    if (index.isValid()) {
        //http://coliru.stacked-crooked.com/a/98276b01d551fb41

        //auto it = std::begin(Segmentation::singleton().objects);
        //std::advance(it, index.row());
        //const auto & obj = it->second;

        //const auto & obj = Segmentation::singleton().objects[objectCache[index.row()]];
        const auto & obj = objectCache[index.row()].get();
        if (index.column() == 0 && role == Qt::BackgroundRole) {
            const auto colorIndex = obj.id % 256;
            const auto red = Segmentation::singleton().overlayColorMap[0][colorIndex];
            const auto green = Segmentation::singleton().overlayColorMap[1][colorIndex];
            const auto blue = Segmentation::singleton().overlayColorMap[2][colorIndex];
            return QColor(red, green, blue);
        } else if (index.column() == 2 && role == Qt::CheckStateRole) {
            return (obj.immutable ? Qt::Checked : Qt::Unchecked);
        } else if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (index.column()) {
            case 1: return static_cast<quint64>(obj.id);
            case 3: return obj.category;
            case 4: return obj.comment;
            case 5: return static_cast<quint64>(obj.subobjects.size());
            case 6: {
                QString output;
                const auto elemCount = std::min(MAX_SHOWN_SUBOBJECTS, obj.subobjects.size());
                for (std::size_t i = 0; i < elemCount; ++i) {
                    output += QString::number(obj.subobjects[i].get().id) + ", ";
                }
                output.chop(2);
                return output;
            }
            }
        }
    }
    return QVariant();//return invalid QVariant
}

bool SegmentationObjectModel::setData(const QModelIndex & index, const QVariant & value, int role) {
    if (index.isValid()) {
        auto & obj = objectCache[index.row()].get();
        if (index.column() == 2 && role == Qt::CheckStateRole) {
            if (!obj.immutable) {//don’t remove immutability
                QMessageBox prompt;
                prompt.setWindowFlags(Qt::WindowStaysOnTopHint);
                prompt.setIcon(QMessageBox::Question);
                prompt.setWindowTitle(tr("Lock Object"));
                prompt.setText(tr("Lock the object with id %0?").arg(obj.id));
                const auto & lockButton = prompt.addButton(tr("Lock"), QMessageBox::YesRole);
                prompt.addButton(tr("Cancel"), QMessageBox::NoRole);
                prompt.exec();
                if (prompt.clickedButton() == lockButton) {
                    obj.immutable = value.toBool();
                }
            }
        } else if (role == Qt::DisplayRole || role == Qt::EditRole) {
            switch (index.column()) {
            case 3: obj.category = value.toString(); break;
            case 4: obj.comment  = value.toString(); break;
            default:
                return false;
            }
        }
    }
    return true;
}

Qt::ItemFlags SegmentationObjectModel::flags(const QModelIndex & index) const {
    Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemNeverHasChildren;//not editable
    switch(index.column()) {
    case 2:
        return flags | Qt::ItemIsUserCheckable;
    case 3:
    case 4:
        return flags | Qt::ItemIsEditable;
    }
    return flags;
}

void SegmentationObjectModel::recreate() {
    beginResetModel();
    objectCache.clear();
    for (auto & pair : Segmentation::singleton().objects) {
        objectCache.emplace_back(pair.second);
    }
    endResetModel();
}

QModelIndex SegmentationObjectModel::index(int row, int column, const QModelIndex &) const {
    return createIndex(row, column);
}

QModelIndex SegmentationObjectModel::parent(const QModelIndex &) const {
    return QModelIndex();
}

void CategoryModel::recreate() {
    beginResetModel();
    categories.clear();
    categories.emplace_back("<category>");
    for (auto & category : Segmentation::singleton().categories) {
        categories.emplace_back(category);
    }
    endResetModel();
}

int CategoryModel::rowCount(const QModelIndex &) const {
    return categories.size();
}

QVariant CategoryModel::data(const QModelIndex &index, int role) const {
    if(role == Qt::DisplayRole) {
        return categories[index.row()];
    }
    return QVariant();
}

SegmentationTab::SegmentationTab(QWidget & parent) : QWidget(&parent) {
    categoryFilter.setModel(&categoryModel);
    categoryModel.recreate();
    categoryFilter.setCurrentIndex(0);
    commentFilter.setPlaceholderText("Filter for comment...");
    showAllChck.setChecked(Segmentation::singleton().renderAllObjs);

    touchedObjsTable.setModel(&touchedObjectModel);
    touchedObjsTable.setAllColumnsShowFocus(true);
    touchedObjsTable.setContextMenuPolicy(Qt::CustomContextMenu);
    touchedObjsTable.setRootIsDecorated(false);
    touchedObjsTable.setSelectionMode(QAbstractItemView::ExtendedSelection);
    touchedObjsTable.setUniformRowHeights(true);//perf hint from doc

    //proxy model chaining, so we can filter twice
    objectProxyModelCategory.setSourceModel(&objectModel);
    objectProxyModelComment.setSourceModel(&objectProxyModelCategory);
    objectProxyModelCategory.setFilterKeyColumn(3);
    objectProxyModelComment.setFilterKeyColumn(4);
    objectsTable.setModel(&objectProxyModelComment);
    objectsTable.setAllColumnsShowFocus(true);
    objectsTable.setContextMenuPolicy(Qt::CustomContextMenu);
    objectsTable.setRootIsDecorated(false);
    objectsTable.setSelectionMode(QAbstractItemView::ExtendedSelection);
    objectsTable.setUniformRowHeights(true);//perf hint from doc
    //sorting seems pretty slow concerning selection and recreation of the model
    //objectsTable.setSortingEnabled(true);
    //objectsTable.sortByColumn(1, Qt::AscendingOrder);

    filterLayout.addWidget(&categoryFilter);
    filterLayout.addWidget(&commentFilter);
    filterLayout.addWidget(&regExCheckbox);

    bottomHLayout.addWidget(&objectCountLabel);
    bottomHLayout.addWidget(&subobjectCountLabel);

    splitter.setOrientation(Qt::Vertical);
    splitter.addWidget(&touchedObjsTable);
    splitter.addWidget(&objectsTable);
    layout.addWidget(&showAllChck);
    layout.addLayout(&filterLayout);
    layout.addWidget(&splitter);
    layout.addLayout(&bottomHLayout);
    setLayout(&layout);

    QObject::connect(&categoryFilter,  &QComboBox::currentTextChanged, this, &SegmentationTab::filter);
    QObject::connect(&commentFilter, &QLineEdit::textEdited, this, &SegmentationTab::filter);
    QObject::connect(&regExCheckbox, &QCheckBox::stateChanged, this, &SegmentationTab::filter);

    for (const auto & index : {0, 1, 2, 5}) {
        //comment (4) shall not waste space, also displaying all supervoxel ids (6) causes problems
        touchedObjsTable.header()->setSectionResizeMode(index, QHeaderView::ResizeToContents);
        objectsTable.header()->setSectionResizeMode(index, QHeaderView::ResizeToContents);
    }

    QObject::connect(&Segmentation::singleton(), &Segmentation::dataChanged, &objectModel, &SegmentationObjectModel::recreate);
    QObject::connect(&Segmentation::singleton(), &Segmentation::dataChanged, this, &SegmentationTab::updateSelection);
    QObject::connect(&Segmentation::singleton(), &Segmentation::dataChanged, &touchedObjectModel, &TouchedObjectModel::recreate);
    QObject::connect(&Segmentation::singleton(), &Segmentation::dataChanged, this, &SegmentationTab::updateTouchedObjSelection);
    QObject::connect(&Segmentation::singleton(), &Segmentation::dataChanged, this, &SegmentationTab::updateLabels);
    QObject::connect(&Segmentation::singleton(), &Segmentation::touchObjectsChanged, &touchedObjectModel, &TouchedObjectModel::recreate);
    QObject::connect(&Segmentation::singleton(), &Segmentation::touchObjectsChanged, this, &SegmentationTab::updateTouchedObjSelection);
    QObject::connect(&Segmentation::singleton(), &Segmentation::selectionChanged, this, &SegmentationTab::updateSelection);
    QObject::connect(&Segmentation::singleton(), &Segmentation::selectionChanged, this, &SegmentationTab::updateTouchedObjSelection);

    QObject::connect(&objectsTable, &QTreeView::customContextMenuRequested, [&](QPoint point){contextMenu(objectsTable, point);});
    QObject::connect(&touchedObjsTable, &QTreeView::customContextMenuRequested, [&](QPoint point){contextMenu(touchedObjsTable, point);});
    QObject::connect(objectsTable.selectionModel(), &QItemSelectionModel::selectionChanged, this, &SegmentationTab::selectionChanged);
    QObject::connect(touchedObjsTable.selectionModel(), &QItemSelectionModel::selectionChanged, this, &SegmentationTab::touchedObjSelectionChanged);
    QObject::connect(&showAllChck, &QCheckBox::clicked, [&](bool value){
        Segmentation::singleton().renderAllObjs = value;
    });

    touchedObjectModel.recreate();
    objectModel.recreate();

    updateLabels();
}

void SegmentationTab::commitSelection(const QItemSelection & selected, const QItemSelection & deselected) {
    for (const auto & index : deselected.indexes()) {
        if (index.column() == 1) {//only evaluate id cell
            Segmentation::singleton().unselectObject(index.data().toInt());
        }
    }
    for (const auto & index : selected.indexes()) {
        if (index.column() == 1) {//only evaluate id cell
            Segmentation::singleton().selectObject(index.data().toInt());
        }
    }
}

void SegmentationTab::touchedObjSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
    if (touchedObjectSelectionProtection) {
        return;
    }

    Segmentation::singleton().blockSignals(true);//prevent ping pong
    if (!QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
        //unselect all selected objects which are not present in the touchedObjectsTable
        auto selection = touchedObjsTable.selectionModel()->selection();
        commitSelection(selection, objectsTable.selectionModel()->selection());
    }
    commitSelection(selected, deselected);
    Segmentation::singleton().blockSignals(false);

    updateSelection();
}

void SegmentationTab::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
    if (objectSelectionProtection) {
        return;
    }
    Segmentation::singleton().blockSignals(true);//prevent ping pong
    commitSelection(selected, deselected);
    Segmentation::singleton().blockSignals(false);
    updateTouchedObjSelection();
}

QItemSelection SegmentationTab::blockSelection(const SegmentationObjectModel & model) {
    QItemSelection selectedItems;

    bool blockSelection = false;
    std::size_t blockStartIndex;

    std::size_t objIndex = 0;
    for (auto & elem : model.objectCache) {
        if (!blockSelection && elem.get().selected) { //start block selection
            blockSelection = true;
            blockStartIndex = objIndex;
        }
        if (blockSelection && !elem.get().selected) { //end block selection
            selectedItems.select(model.index(blockStartIndex, 0), model.index(objIndex-1, model.columnCount()-1));
            blockSelection = false;
        }
        ++objIndex;
    }
    //finish last blockselection – if any
    if (blockSelection) {
        selectedItems.select(model.index(blockStartIndex, 0), model.index(objIndex-1, model.columnCount()-1));
    }

    return selectedItems;
}

void SegmentationTab::updateTouchedObjSelection() {
    const auto & selectedItems = blockSelection(touchedObjectModel);

    touchedObjectSelectionProtection = true;//using block signals prevents update of the tableview
    touchedObjsTable.clearSelection();
    touchedObjsTable.selectionModel()->select(selectedItems, QItemSelectionModel::Select);
    touchedObjectSelectionProtection= false;
}

void SegmentationTab::updateSelection() {
    const auto & selectedItems = blockSelection(objectModel);
    const auto & proxySelection = objectProxyModelComment.mapSelectionFromSource(objectProxyModelCategory.mapSelectionFromSource(selectedItems));

    objectSelectionProtection = true;//using block signals prevents update of the tableview
    objectsTable.clearSelection();
    objectsTable.selectionModel()->select(proxySelection, QItemSelectionModel::Select);
    objectSelectionProtection = false;

    if (!selectedItems.indexes().isEmpty()) {// scroll to first selected entry
        objectsTable.scrollTo(selectedItems.indexes().front());
    }
}

void SegmentationTab::filter() {
    if (categoryFilter.currentText() != "<category>") {
        objectProxyModelCategory.setFilterFixedString(categoryFilter.currentText());
    } else {
        objectProxyModelCategory.setFilterFixedString("");
    }
    if (regExCheckbox.isChecked()) {
        objectProxyModelComment.setFilterRegExp(commentFilter.text());
    } else {
        objectProxyModelComment.setFilterFixedString(commentFilter.text());
    }
    updateSelection();
    updateTouchedObjSelection();
}

void SegmentationTab::updateCategories() {
    categoryModel.recreate();
    categoryFilter.setCurrentIndex(0);
}

void SegmentationTab::updateLabels() {
    objectCountLabel.setText(QString("Objects: %0").arg(Segmentation::singleton().objects.size()));
    subobjectCountLabel.setText(QString("Subobjects: %0").arg(Segmentation::singleton().subobjects.size()));
}

void SegmentationTab::contextMenu(const QAbstractScrollArea & widget, const QPoint & pos) {
    QMenu contextMenu;
    QObject::connect(contextMenu.addAction("merge"), &QAction::triggered, &Segmentation::singleton(), &Segmentation::mergeSelectedObjects);
    QObject::connect(contextMenu.addAction("delete"), &QAction::triggered, &Segmentation::singleton(), &Segmentation::deleteSelectedObjects);
    contextMenu.exec(widget.viewport()->mapToGlobal(pos));
}

