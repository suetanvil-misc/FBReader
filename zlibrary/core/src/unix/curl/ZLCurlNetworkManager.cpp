/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#include <ZLOutputStream.h>

#include "ZLCurlNetworkManager.h"
#include "ZLCurlNetworkDownloadData.h"
#include "ZLCurlNetworkXMLParserData.h"

void ZLCurlNetworkManager::createInstance() {
	ourInstance = new ZLCurlNetworkManager();
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createDownloadData(const std::string &url, const std::string &fileName, const std::string &sslCertificate, shared_ptr<ZLOutputStream> stream) const {
	return new ZLCurlNetworkDownloadData(url, fileName, sslCertificate, stream);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createDownloadData(const std::string &url, const std::string &fileName, shared_ptr<ZLOutputStream> stream) const {
	return new ZLCurlNetworkDownloadData(url, fileName, stream);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createXMLParserData(const std::string &url, const std::string &sslCertificate, shared_ptr<ZLXMLReader> reader) const {
	return new ZLCurlNetworkXMLParserData(url, sslCertificate, reader);
}

shared_ptr<ZLExecutionData> ZLCurlNetworkManager::createXMLParserData(const std::string &url, shared_ptr<ZLXMLReader> reader) const {
	return new ZLCurlNetworkXMLParserData(url, reader);
}