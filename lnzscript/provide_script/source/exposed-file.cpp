
///Function:File.cd
///Arguments:string strDirectory=""
///Returns:bool bSuccess
///Doc:Change the current directory. This affects subsequent calls to functions that affect files. If no argument is given, returns the current working directory.
///Implementation:c++_qt
{
	CHECK_ARGS
	// luckily, this seems to affect the current dir for the Au3 functions as well, like iniRead and so on
	if (strDirectory!="") // change the current directory
	{
		bool bResult = QDir::setCurrent(strDirectory);
		if (!bResult) return ctx->throwError("File.cd(): directory may not exist.");
		else return QScriptValue(eng, true);
	}
	else // return the current directory
	{
		QFileInfo objFileInfo("."); //refers to current dir.
		return QScriptValue(eng, util_fixQtDirectorySlashes(objFileInfo.absoluteFilePath())); 
	}
}
///Function:File.readFile
///Arguments:string strFile, bool bTextMode=true
///Returns:string strContents
///Doc:Read text from a file. (Not recommended for binary files).Throws exception if file not found or is not readable. If bTextMode is true, \r\n characters on Windows are converted to \n
///Implementation:c++_qt
{
	CHECK_ARGS
	QFile file(strFile);
	QString strContents;
	QFlags<QIODevice::OpenModeFlag> flags = bTextMode ? (QIODevice::ReadOnly | QIODevice::Text) : QIODevice::ReadOnly;
	try
	{
		bool bResult = file.open(flags); if (!bResult) return ctx->throwError("File.readFile() could not open file for reading.");
		strContents = file.readAll();
	} catch (...) { file.close();  return ctx->throwError("File.readFile() unknown error!"); }
	file.close();
	return QScriptValue(eng, strContents); 
}

///Function:File.writeFile
///Arguments:string strFile, string strText, bool bAppend=false, bool bTextMode=true
///Returns:
///Doc:Write text to a file. (Not recommended for binary files). Throws exception if file is not writeable. If bTextMode is true, newline characters are converted according to platform.
///Implementation:c++_qt
{
	CHECK_ARGS
	QFile file(strFile);
	QFlags<QIODevice::OpenModeFlag> flags = bTextMode ? (QIODevice::WriteOnly | QIODevice::Text) : QIODevice::WriteOnly;
	if (bAppend) flags = flags | QIODevice::Append;
	try
	{
		bool bResult = file.open(flags); if (!bResult) return ctx->throwError("File.writeFile() could not open file for write.");
		qint64 nBytesWritten = file.write(QStrToCStr(strText)); if (nBytesWritten==-1) return ctx->throwError("File.writeFile() write failed.");
	} catch (...) { file.close();  return ctx->throwError("File.writeFile() unknown error!"); }
	file.close();
	return QScriptValue(eng, true); 
}


///Function:File.exists
///Arguments:string strFile
///Returns:bool bExists
///Doc:Test if the file or directory exists.
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFile);
	return objFileInfo.exists() ? QScriptValue(eng, true) : QScriptValue(eng, false); 
}
///Function:File.isReadable
///Arguments:string strFile
///Returns:bool bReadable
///Doc:Test if the current user has read permissions for the file.
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFile); if (!objFileInfo.exists()) return ctx->throwError("File does not exist.");
	return objFileInfo.isReadable() ? QScriptValue(eng, true) : QScriptValue(eng, false); 
}
///Function:File.isWritable
///Arguments:string strFile
///Returns:bool bWritable
///Doc:Test if the current user has read permissions for the file.
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFile); if (!objFileInfo.exists()) return ctx->throwError("File does not exist.");
	return objFileInfo.isWritable() ? QScriptValue(eng, true) : QScriptValue(eng, false); 
}
///Function:File.isDirectory
///Arguments:string strFilepath
///Returns:bool bDirectory
///Doc:Test if it is a directory (folder).
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFilepath); if (!objFileInfo.exists()) return ctx->throwError("File does not exist.");
	return objFileInfo.isDir() ? QScriptValue(eng, true) : QScriptValue(eng, false); 
}

///Function:File.getAbsPath
///Arguments:string strFilepath
///Returns:string strAbsPath
///Doc:Find absolute path from a relative path.
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFilepath); if (!objFileInfo.exists()) return ctx->throwError("File does not exist.");
	return QScriptValue(eng, util_fixQtDirectorySlashes(objFileInfo.absoluteFilePath())); 
}

///Function:File.getSize
///Arguments:string strFile
///Returns:int nSizeInBytes
///Doc:Returns the file size in bytes. (May not work for files > 2 Gb in size).
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFile); if (!objFileInfo.exists()) return ctx->throwError("File does not exist.");
	qint64 nBytes = objFileInfo.size();
	if (nBytes > UINT_MAX) return ctx->throwError("File.getSize(). File is too large, possibly > 2 Gb.");
	return QScriptValue(eng,(uint) nBytes); 
}

///Function:File.getTimeCreated
///Arguments:string strFile
///Returns:int nSecondsSince1970
///Doc:Returns the date and time when the file was created, as an integer. (Seconds since 1970-01-01 UTC).
///Example: var d = new Date(); d.setTime(File.getTimeCreated('test.txt')*1000);
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFile); if (!objFileInfo.exists()) return ctx->throwError("File does not exist.");
	return QScriptValue(eng, objFileInfo.created().toTime_t()); 
}
///Function:File.getTimeModified
///Arguments:string strFile
///Returns:int nSecondsSince1970
///Doc:Returns the date and time when the file was last modified, as an integer. (Seconds since 1970-01-01 UTC).
///Example: var d = new Date(); d.setTime(File.getTimeModified('test.txt')*1000);
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFile); if (!objFileInfo.exists()) return ctx->throwError("File does not exist.");
	return QScriptValue(eng, objFileInfo.lastModified().toTime_t()); 
}
///Function:File.getTimeAccessed
///Arguments:string strFile
///Returns:int nSecondsSince1970
///Doc:Returns the date and time when the file was last accessed, as an integer. (Seconds since 1970-01-01 UTC). On platforms where this information is not available, returns the same as Modified().
///Example: var d = new Date(); d.setTime(File.getTimeAccessed('test.txt')*1000);
///Implementation:c++_qt
{
	CHECK_ARGS
	QFileInfo objFileInfo(strFile); if (!objFileInfo.exists()) return ctx->throwError("File does not exist.");
	return QScriptValue(eng, objFileInfo.lastRead().toTime_t()); 
}


///Function:File.createDirectory
///Arguments:string strDirectory
///Returns:bool bSuccess
///Doc:Create an empty directory, by default in the current directory (which can be changed with File.cd() ).
///Implementation:c++_qt
{
	CHECK_ARGS
	QDir objDir; //QDir objDir(strParentFolder);
	bool bResult = objDir.mkdir(strDirectory);
	return (bResult) ? QScriptValue(eng, true) : QScriptValue(eng, false);
}
///Function:File.removeDirectory
///Arguments:string strDirectory
///Returns:bool bSuccess
///Doc:Remove an empty directory, by default in the current directory (which can be changed with File.cd() ).
///Implementation:c++_qt
{
	CHECK_ARGS
	QDir objDir; //QDir objDir(strParentFolder);
	bool bResult = objDir.rmdir(strDirectory);
	return (bResult) ? QScriptValue(eng, true) : QScriptValue(eng, false);
}

///Function:File.dirCountEntries
///Arguments:string strDirectory
///Returns:int nEntries
///Doc:Counts how many files and subfolders are in the folder.
///Implementation:c++_qt
{
	CHECK_ARGS
	QDir objDir(strDirectory); 
	if (! objDir.exists()) return ctx->throwError("File.dirCountEntries(). No directory found with that name.");
	// Note: we subtract two because of the special entries . and ..
	return QScriptValue(eng, objDir.count()-2);
}
///Function:File.dirListFiles
///Arguments:string strDir, string strTypes="*", string strSortBy="Unsorted"
///Returns:arFiles
///Doc:Returns array of filenames inside folder. Provide a pattern like '*.txt' to only include .txt files. By default unsorted, but provide a flag such as 'unsorted', 'name','time','size','type'. Includes files marked as hidden, but not those marked as system.
///Example:var arFiles = File.dirListFiles('c:\\','*','size');[[br]] print('biggest files in c:');[[br]] arFiles.reverse();[[br]] for(var i=0;i<arFiles.length;i++) print(arFiles[i]);
///Implementation:c++_qt
{
	CHECK_ARGS
	QDir objDir(strDir); 
	if (! objDir.exists()) return ctx->throwError("File.dirListFiles(). No directory found with that name.");
	QStringList filters;
	if (!strTypes.isEmpty() && strTypes!="*") {
		filters << strTypes;
		objDir.setNameFilters(filters);
	}
	
	objDir.setFilter(QDir::Files | QDir::Hidden);
	return util_ListDirectoryEntries(ctx, eng, objDir, strSortBy);
}
///Function:File.dirListSubdirs
///Arguments:string strDir, string strSortBy="Unsorted"
///Returns:arDirs
///Doc:Returns array of folder names that are subfolders of the folder. By default unsorted, but provide a flag such as 'unsorted', 'name','time','type'. Includes files marked as hidden, but not those marked as system.
///Example:var arFolders = File.dirListSubdirs('c:\\','name');[[br]] print('subfolders in c:');[[br]] for(var i=0;i<arFolders.length;i++) print(arFolders[i]);
///Implementation:c++_qt
{
	CHECK_ARGS
	QDir objDir(strDir); 
	if (! objDir.exists()) return ctx->throwError("File.dirListSubdirs(). No directory found with that name.");
	
	objDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden); // and QDir::Drives ?
	return util_ListDirectoryEntries(ctx, eng, objDir, strSortBy);
}

///Function:File.getPathHome
///Arguments:
///Returns:string strPath
///Doc:Return the directory of the current user's profile, typically something like C:/Documents and Settings/Username. If nothing is found, may fall back to c:\
///Implementation:c++_qt
{
	CHECK_ARGS
	return QScriptValue(eng, util_fixQtDirectorySlashes(QDir::homePath())); 
}
///Function:File.getPathTemp
///Arguments:
///Returns:string strPath
///Doc:Return the system's temporary directory.
///Implementation:c++_qt
{
	CHECK_ARGS
	return QScriptValue(eng, util_fixQtDirectorySlashes(QDir::tempPath())); 
}

///Function:File.getPathSpecial
///Arguments:SpecialFolder folder
///Returns:string strPath
///Doc:Get the path of a special folder, such as SpecialFolder.MyDocuments. See also openExplorerWindowSpecial. Example: File.getPathSpecial(SpecialFolder.AppData); Also: [[br]]SpecialFolder.AppData[[br]] SpecialFolder.AllUsers[[br]]SpecialFolder.Cookies[[br]]SpecialFolder.Desktop[[br]]SpecialFolder.Favorites[[br]]SpecialFolder.AppDataLocalSettings[[br]]SpecialFolder.MyDocuments[[br]]SpecialFolder.MyPictures[[br]]SpecialFolder.ProgramFiles[[br]]SpecialFolder.ProgramFilesCommon[[br]]SpecialFolder.RecentDocuments[[br]]SpecialFolder.StartMenu[[br]]SpecialFolder.StartupItems[[br]]SpecialFolder.System[[br]]SpecialFolder.Windows[[br]]
///Implementation:c++_winext
{
	CHECK_ARGS
	QString ret = get_winapi_special_folder_path(folder);
	if (ret=="") return ctx->throwError("File.getPathSpecial(). Unrecognized folder name. See documentation.");
	
	return QScriptValue(eng, ret); 
}

///Function:File.openExplorerWindowSpecial
///Arguments:SpecialFolder folder
///Returns:
///Doc:Open a folder in explorer, such as SpecialFolder.MyDocuments. See also getPathSpecial. Supports all available in File.getPathSpecial and also: [[br]]SpecialFolder.ControlPanel[[br]]SpecialFolder.Fonts[[br]]SpecialFolder.Printers[[br]]SpecialFolder.MyComputer[[br]]SpecialFolder.MyDocuments[[br]]SpecialFolder.MyNetworkPlaces[[br]]SpecialFolder.NetworkComputers[[br]]SpecialFolder.NetworkConnections[[br]]SpecialFolder.PrintersAndFaxes[[br]]SpecialFolder.RecycleBin[[br]]SpecialFolder.ScheduledTasks
///Implementation:c++_winext
{
	CHECK_ARGS
	if (folder=="Control Panel" || folder=="Fonts" || folder=="Printers")
	{
		QString cmd;
		if (folder=="Control Panel") cmd = "Control_RunDLL";
		else if (folder=="Fonts") cmd = "SHHelpShortcuts_RunDLL FontsFolder";
		else if (folder=="Printers") cmd = "SHHelpShortcuts_RunDLL PrintersFolder";
		else return ctx->throwError("File.openExplorerWindowSpecial(). Internal error.");
		return util_runExternalCommand("rundll32.exe shell32.dll,"+cmd);
	}
	else
	{
		QString clsId;
		if (folder=="My Computer") clsId="::{20d04fe0-3aea-1069-a2d8-08002b30309d}";
		else if (folder=="My Documents") clsId="::{450d8fba-ad25-11d0-98a8-0800361b1103}";
		else if (folder=="My Network Places") clsId="::{208d2c60-3aea-1069-a2d7-08002b30309d}";
		else if (folder=="Network Computers") clsId="::{1f4de370-d627-11d1-ba4f-00a0c91eedba}";
		else if (folder=="Network Connections") clsId="::{7007acc7-3202-11d1-aad2-00805fc1270e}";
		else if (folder=="Printers and Faxes") clsId="::{2227a280-3aea-1069-a2de-08002b30309d}";
		else if (folder=="Recycle Bin") clsId="::{645ff040-5081-101b-9f08-00aa002f954e}";
		else if (folder=="Scheduled Tasks") clsId="::{d6277990-4c6a-11cf-8d87-00aa0060f5bf}";
		else 
		{
			// try using getPathSpecial. This will just open a normal directory in the typical manner.
			QString ret = get_winapi_special_folder_path(folder);
			if (ret=="") 
				return ctx->throwError("File.openExplorerWindowSpecial(). Unrecognized folder name. See documentation.");
			else
				return util_runExternalCommand("cmd.exe /c start \"" + ret + "\""); //the cmd.exe should spawn off what we opened.
		}
		return util_runExternalCommand("cmd.exe /c start " + clsId); //the cmd.exe should spawn off what we opened.
	}
}



///Function:File.getShortcutTarget
///Arguments:string strFilename
///Returns:string strTarget
///Doc:Find the target of a shortcut (.lnk on Windows).
///Implementation:c++_qt
///Example:var strTarget = File.getShortcutTarget('shortcut.lnk'); if (File.exists(strTarget)) print('target exists');
{
	CHECK_ARGS
	if (! QFileInfo(strFilename).exists()) return ctx->throwError("File.getShortcutTarget(). File does not exist.");
	QString strTarget = QFile::symLinkTarget(strFilename);
	if (strTarget=="")
		return ctx->throwError("File.getShortcutTarget(). File is not a valid shortcut.");
	else
		return QScriptValue(eng, util_fixQtDirectorySlashes(strTarget)); 
}
///Function:File.createShortcut
///Arguments:string strFilename, string strTarget
///Returns:bool bSuccess
///Doc:Create a shortcut (.lnk on Windows). Will not replace an existing file.
///Implementation:c++_qt
///Example:File.createShortcut('shortcut.lnk','c:\\program files\\my program\\program.exe');
{
	CHECK_ARGS
	//if (! QFileInfo(strTarget).exists()) return ctx->throwError("File.createShortcut(). Target does not exist.");
	// it could be a non-file like program /f something.txt
	// this works for a simple file/program but NOT for something with arguments.
	// we might have to rewrite using win api.
	bool bResult = QFile::link(strTarget, strFilename);
	return bResult ? QScriptValue(eng, true): QScriptValue(eng, false); 
}
///Function:File.copy
///Arguments:string strFilename, string strCopyname
///Returns:bool bSuccess
///Doc:Create a copy of a file. Will not replace an existing file.
///Implementation:c++_qt
{
	CHECK_ARGS
	if (! QFileInfo(strFilename).exists()) return ctx->throwError("File.copy(). File does not exist.");
	bool bResult = QFile::copy(strFilename, strCopyname);
	return bResult ? QScriptValue(eng, true): QScriptValue(eng, false); 
}
///Function:File.rename
///Arguments:string strFilename, string strNewname
///Returns:bool bSuccess
///Doc:Rename a file. Will not overwrite an existing file.
///Implementation:c++_qt
{
	CHECK_ARGS
	if (! QFileInfo(strFilename).exists()) return ctx->throwError("File.rename(). File does not exist.");
	bool bResult = QFile::rename(strFilename, strNewname);
	return bResult ? QScriptValue(eng, true): QScriptValue(eng, false); 
}
///Function:File.removeFile
///Arguments:string strFilename
///Returns:bool bSuccess
///Doc:Delete a file. Returns false if delete does not succeed.
///Implementation:c++_qt
{
	CHECK_ARGS
	if (! QFileInfo(strFilename).exists()) return ctx->throwError("File.removeFile(). File does not exist.");
	bool bResult = QFile::remove(strFilename);
	return bResult ? QScriptValue(eng, true): QScriptValue(eng, false); 
}

///Function:File.showInExplorer
///Arguments:string strFilename
///Returns:
///Doc:Opens new explorer window, and highlights file.
///Implementation:c++_winext
{
	CHECK_ARGS
	if (! QFileInfo(strFilename).exists()) return ctx->throwError("File.showInExplorer(). File does not exist.");
	
	QString strExecutable; strExecutable = "explorer /e,/select,\"" + util_external_escape(strFilename) + "\"";
	return util_runExternalCommand(strExecutable);
}

///Function:File.openExplorerWindow
///Arguments:string strDirectory, bool bTreeView=false
///Returns:
///Doc:Opens new explorer window.
///Implementation:c++_winext
{
	CHECK_ARGS
	if (! QDir(strDirectory).exists()) return ctx->throwError("File.openExplorerWindow(). Directory does not exist.");
	
	QString strExecutable; 
	strExecutable = (! bTreeView) ? "explorer \"" : "explorer /e,\"";
	strExecutable += util_external_escape(strDirectory) + "\"";
	return util_runExternalCommand(strExecutable);
}


///Function:File.driveMapDialog
///Arguments:
///Returns:
///Doc:Opens standard "Map Network Drive" dialog.
///Implementation:c++_winext
{
	CHECK_ARGS
	return util_runExternalCommand("RunDll32.exe shell32.dll,SHHelpShortcuts_RunDLL Connect");
}


// Here are the Au3 ones:

///Function:File.driveMapAdd
///Arguments:string strDevice, string strRemoteShare, bool bAuthDialog=0, string strUser="", string strPass=""
///Returns:bool bR
///Doc:Maps a network drive. strDevice is the device to map, for example "O:" or "LPT1:", or "" which makes a connection that is not mapped to a specific drive, or "*" to select an unused drive letter. strRemoteShare, in form "\\server\share". strUser, in form "username" or "domain\username"
///Example: File.driveMapAdd("K:","\\\\myserver\\folder")
///Implementation:c++_au3
{
	CHECK_ARGS
	char buf[SMALLBUFSIZE];
	long nFlags = (bAuthDialog) ? 0+8 : 0;
	AU3_DriveMapAdd(QStrToCStr(strDevice),QStrToCStr(strRemoteShare), nFlags, QStrToCStr(strUser), QStrToCStr(strPass),buf, SMALLBUFSIZE);
	// disregard string result... there's not an easy way to return it...
	if (AU3_error()==0) return QScriptValue(eng, true); //no error
	else return QScriptValue(eng, false); //an error
}

///Function:File.driveMapAddPersistant
///Arguments:string strDevice, string strRemoteShare, bool bAuthDialog=0, string strUser="", string strPass=""
///Returns:bool bR
///Doc:Maps a network drive. strDevice is the device to map, for example "O:" or "LPT1:", or "" which makes a connection that is not mapped to a specific drive, or "*" to select an unused drive letter. strRemoteShare, in form "\\server\share". strUser, in form "username" or "domain\username"
///Implementation:c++_au3
{
	CHECK_ARGS
	char buf[SMALLBUFSIZE];
	long nFlags = (bAuthDialog) ? 1+8 : 1;
	AU3_DriveMapAdd(QStrToCStr(strDevice),QStrToCStr(strRemoteShare), nFlags, QStrToCStr(strUser), QStrToCStr(strPass),buf, SMALLBUFSIZE);
	// disregard string result... there's not an easy way to return it...
	if (AU3_error()==0) return QScriptValue(eng, true); //no error
	else return QScriptValue(eng, false); //an error
}

///Function:File.driveMapDisconnect
///Arguments:string strDrive
///Returns:bool bSuccess
///Doc:Disconnects network drive. Provide drive letter "X:" or, if necessary, connection name "\\server\share".
///Implementation:c++_au3
{
	CHECK_ARGS
	long nRes = AU3_DriveMapDel(QStrToCStr(strDrive));
	return util_LongToBool(nRes);
}

///Function:File.driveMapInformation
///Arguments:string strDrive
///Returns:string strInformation
///Doc:Retreives the details of a mapped drive. Returns details of the mapping, e.g. \\server\share. Returns number -1 upon failure.
///Example:print(File.driveMapInformation('X:'));
///Implementation:c++_au3
{
	CHECK_ARGS
	char buf[BUFSIZE];
	
	AU3_DriveMapGet(QStrToCStr(strDrive), buf, BUFSIZE);
	if (AU3_error()==1)
		return QScriptValue(eng, -1);
	else
		return QScriptValue(eng, QString(buf));
}

///Function:File.iniReadVal
///Arguments:string strFilename, string strSection, string strKey
///Returns:string strValue
///Doc:Reads a value from a standard format .ini file. If the key is not found, returns false. Reads 1024 bytes maximum.
///Implementation:c++_au3
{
	CHECK_ARGS
	#define NOTFOUNDFLAG "~~~Lnz_iniRead_notfound~~~"
	char buf[BUFSIZE];
	
	AU3_IniRead(QStrToCStr(strFilename), QStrToCStr(strSection),QStrToCStr(strKey),NOTFOUNDFLAG, buf, BUFSIZE);
	if (strcmp(buf, NOTFOUNDFLAG)==0 || !AU3_error()) // wasn't found
		return QScriptValue(eng, false);
	else
		return QScriptValue(eng, QString(buf));
}

///Function:File.iniDeleteVal
///Arguments:string strFilename, string strSection, string strKey
///Returns:bool bSuccess
///Doc:Deletes a value from a standard format .ini file. Returns false if section/key is not found or if INI file is read-only.
///Implementation:c++_au3
{
	CHECK_ARGS
	long nRes = AU3_IniDelete(QStrToCStr(strFilename), QStrToCStr(strSection),QStrToCStr(strKey));
	return QScriptValue(eng, ((nRes==1) && (AU3_error()!=0)));
}

///Function:File.iniDeleteSection
///Arguments:string strFilename, string strSection
///Returns:bool bSuccess
///Doc:Deletes a section from a standard format .ini file. Returns false if section is not found or if INI file is read-only.
///Implementation:c++_au3
{
	CHECK_ARGS
	long nRes = AU3_IniDelete(QStrToCStr(strFilename), QStrToCStr(strSection),"");
	return QScriptValue(eng, ((nRes==1) && (AU3_error()!=0)));
}

///Function:File.iniWriteVal
///Arguments:string strFilename, string strSection, string strKey, string strValue
///Returns:bool bSuccess
///Doc:Writes a value to a standard format .ini file. If file does not exist, it is created. Keys and/or sections are added to the end and are not sorted in any way. Returns false if file is read-only.
///Implementation:c++_au3
{
	CHECK_ARGS
	long nRes = AU3_IniWrite(QStrToCStr(strFilename), QStrToCStr(strSection),QStrToCStr(strKey),QStrToCStr(strValue));
	return util_LongToBool(nRes);
}

///Function:File.tray
///Arguments:string strDriveLetter, string strOpenOrClosed
///Returns:bool bSuccess
///Doc:Opens or closes the CD Tray. Provide the string 'open' or 'closed'. Drive letter in format 'D:' or 'd:'. Works as expected with virtual cd drives.
///Example: File.tray('D:', 'open'); File.tray('D:', 'closed');
///Implementation:c++_au3
{
	CHECK_ARGS
	if (!(strOpenOrClosed=="open" || strOpenOrClosed=="closed")) return ctx->throwError("File.tray(), must be one of 'open' or 'closed'");
	long nRes = AU3_CDTray(QStrToCStr(strDriveLetter), QStrToCStr(strOpenOrClosed));
	return util_LongToBool(nRes);
}

///Function:File.pathSplit
///Arguments:string strPath
///Returns:[string strPath, string strFile]
///Doc: (Write include('<std>') to import this function.) Splits a directory into two parts. 1) the path, and 2) the filename. For example, 'c:\docs\foo\test.txt' to ['c:\docs\foo\', 'test.txt']
///Implementation:Javascript
{}

///Function:File.pathJoin
///Arguments:string strPath, string strFilename
///Returns:string strCompletePath
///Doc: (Write include('<std>') to import this function.) Combines a directory and filename to create a path. For example, File.pathJoin('c:\docs\foo\', 'test.txt') becomes 'c:\docs\foo\test.txt'
///Implementation:Javascript
{}


///Function:File.emptyRecycleBin
///Arguments:string strDrive=""
///Returns:bool bStatus
///Doc:Empty recycling bin. Optionally specify a drive, such as "c:"
///Example:Screen.convertImages('c:\\myfolder\\*.bmp','.png') //converts all .bmp images in the folder to .png.
///Implementation:c++_nir-cmd


///Function:File.copyMany
///Arguments:string strSource, string strDestination
///Returns:
///Doc:Copy an entire directory to another location. One can also copy many files, specifying '*.txt' to copy all text files in the current directory. May sometimes display a window.
///Example:File.copyMany('c:\\temp1\\folder1','c:\\temp2\\'); /* copies entire folder. */[[br]] File.copyMany('c:\\temp\\*.*','d:\\destination');
///Implementation:c++_qt
{
	CHECK_ARGS
	QStringList args;
	args << "xcopy" << strSource << strDestination;
	args << "/s" << "/e" << "/h"; // copy recursively, include empty dirs, include hidden files
	args << "/q" << "/y"; // suppress messages, suppress confirmation 
	int nExitCode=0;
	util_runCommandAndWait(args,&nExitCode);
	return util_LongToBool(!nExitCode);
}







