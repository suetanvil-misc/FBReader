/*
 * Copyright (C) 2004-2006 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "ZLDialogManager.h"
#include "ZLSelectionDialog.h"

ZLTreeHandler::ZLTreeHandler() {
}

ZLTreeHandler::~ZLTreeHandler() {
}

ZLSelectionDialog::ZLSelectionDialog(ZLTreeHandler &handler) : myHandler(handler) {
}

ZLSelectionDialog::~ZLSelectionDialog() {
}

void ZLSelectionDialog::runNode(const ZLTreeNodePtr node) {
	const std::string selectedName = myHandler.relativeId(*node);
	if (node->isFolder()) {
		myHandler.changeFolder(*node);
		update(selectedName);
	} else {
		const std::string message = myHandler.accept(*node);
		if (!message.empty()) {
			ZLDialogManager::instance().infoBox(ZLDialogManager::ERROR_TYPE, "Error", message, "Ok");
		} else if (myHandler.isWriteable()) {
			update(selectedName);
		} else {
			exitDialog();
		}
	}
}

ZLTreeNode::ZLTreeNode(const std::string &id, const std::string &displayName, const std::string &pixmapName, bool isFolder) : myId(id), myDisplayName(displayName), myPixmapName(pixmapName), myIsFolder(isFolder) {
}

bool ZLTreeNode::isFolder() const {
	return myIsFolder;
}