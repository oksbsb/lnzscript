

///Function:Dialog.toolTip
///Arguments:string strText, int x=INTDEFAULT, int y=INTDEFAULT
///Returns:
///Doc:Creates a tooltip anywhere on the screen. If the x and y coordinates are omitted, the tip is placed near the mouse cursor. If the coords would cause the tooltip to run off screen, it is repositioned to visible. Tooltip appears until it is cleared, until script terminates, or sometimes until it is clicked upon. You may use a linefeed character to create multi-line tooltips.
///Example: Dialog.toolTip("That's cool");[[br]] Time.sleep(500);[[br]] Dialog.toolTip("");
///Implementation:c++_au3
{
	CHECK_ARGS
	
	AU3_ToolTip(QStrToCStr(strText), x, y);
	return eng->nullValue();
}

// The reason why I don't use Qt for these is that it would create a dependency on QtGui.dll, which is pretty heavy.
// So, I wrote WinCommonDialog, which wraps Windows MessageBox.

///Function:Dialog.alert
///Arguments:string strTitle, string strText
///Returns:
///Doc:Display simple message box on the screen.
///Example:Dialog.alert("Information","Hello, world");
///Implementation:c++_winext
{
	CHECK_ARGS
	
	return R_WinCommonDialog("simple", "info", strTitle, strText);
}

///Function:Dialog.warning
///Arguments:string strTitle, string strText
///Returns:
///Doc:Display simple message box on the screen, warning icon.
///Implementation:c++_winext
{
	CHECK_ARGS
	return R_WinCommonDialog("simple", "warning", strTitle, strText);
}
///Function:Dialog.error
///Arguments:string strTitle, string strText
///Returns:
///Doc:Display simple message box on the screen, error icon.
///Implementation:c++_winext
{
	CHECK_ARGS
	return R_WinCommonDialog("simple", "error", strTitle, strText);
}
///Function:Dialog.askYesNo
///Arguments:string strTitle, string strText
///Returns:Dialog.YES or Dialog.NO
///Doc:Display simple message box on the screen, with options for yes and no.
///Implementation:c++_winext
///Example:var res = Dialog.askYesNo("Status","Continue?");[[br]] if (res==Dialog.YES) doSomething();[[br]] else doSomethingElse();
{
	CHECK_ARGS
	return R_WinCommonDialog("simple", "yesno", strTitle, strText);
}
///Function:Dialog.askYesNoCancel
///Arguments:string strTitle, string strText
///Returns:Dialog.YES;Dialog.NO;Dialog.CANCEL
///Doc:Display simple message box on the screen, with options for yes, no, cancel.
///Implementation:c++_winext
///Example:var res = Dialog.askYesNoCancel("Warning","Replace existing file?");[[br]]if (res==Dialog.YES) doSomething();[[br]]else if (res==Dialog.NO) doSomethingElse();[[br]]else doCancel();
{
	CHECK_ARGS
	return R_WinCommonDialog("simple", "yesnocancel", strTitle, strText);
}

///Function:Dialog.askOkCancel
///Arguments:string strTitle, string strText
///Returns:Dialog.OK or Dialog.CANCEL
///Doc:Display simple message box on the screen, with options for ok and cancel.
///Implementation:c++_winext
///Example:var res = Dialog.askOkCancel("Status","Continue?");[[br]] if (res==Dialog.OK) doSomething();[[br]] else doSomethingElse();
{
	CHECK_ARGS
	return R_WinCommonDialog("simple", "okcancel", strTitle, strText);
}

///Function:Dialog.askColor
///Arguments:
///Returns:array rgb
///Doc:Opens Windows color picker. Returns results as r,g,b values from 0-255. Returns false if canceled or timeout.
///Implementation:c++_winext
{
	CHECK_ARGS
	
	QStringList args; args << "color";
	QString strOutput = util_externalCmdStdout(G_WinCommonDialog, args);
	if (!strOutput.size()) return QScriptValue(eng, false);
	
	bool ok;
	long color = strOutput.toLong(&ok); 
	if (!ok) return ctx->throwError("Internal error. Dialog color; couldn't read output as long.");
	
	// translate to RGB using Windows api macros GetRValue
	QScriptValue ar = eng->newArray(3);
	ar.setProperty(0, QScriptValue(eng, GetRValue(color)));
	ar.setProperty(1, QScriptValue(eng, GetGValue(color)));
	ar.setProperty(2, QScriptValue(eng, GetBValue(color)));
	return ar;
}

///Function:Dialog.openFile
///Arguments:string strFiletype="*", bool bMult=false, string strStartDir="."
///Returns:string strFile
///Doc:Opens standard Open File dialog. Provide type in format such as 'bmp', NOT '*.bmp'. If not multiple, returns string of filepath or false on cancel or timeout. If multiple, returns an array of strings: if the user chooses one file, ar[0] is the entire filepath and file name. if the user chooses > one file, ar[0] is the directory, and ar[1] to the rest are only the file names.
///Implementation:c++_winext
{
	CHECK_ARGS
	if (strFiletype.contains("*.", Qt::CaseSensitive)) return ctx->throwError("Dialog.openFile(): Provide type in format 'bmp', NOT '*.bmp'.");
	
	QStringList args; 
	args << "file" << (bMult ? "openmult" : "open") << strFiletype << strStartDir;
	QString strOutput = util_externalCmdStdout(G_WinCommonDialog, args);
	if (!strOutput.size()) return QScriptValue(eng, false);
	
	if (bMult)
	{
		QStringList list = strOutput.split("\n", QString::KeepEmptyParts);
		return util_QListToScriptArray(eng, list);
	}
	else
		return QScriptValue(eng, strOutput);
}

///Function:Dialog.saveFile
///Arguments:string strFiletype="*", string strStartDir="."
///Returns:string strFile
///Doc:Opens standard Save File dialog. Provide type in format such as 'bmp', NOT '*.bmp'. Returns string of filepath or false on cancel or timeout.
///Implementation:c++_winext
{
	CHECK_ARGS
	if (strFiletype.contains("*.", Qt::CaseSensitive)) return ctx->throwError("Dialog.saveFile(): Provide type in format 'bmp', NOT '*.bmp'.");
	
	QStringList args; 
	args << "file" << "save" << strFiletype <<  strStartDir;
	QString strOutput = util_externalCmdStdout(G_WinCommonDialog, args);
	if (!strOutput.size()) return QScriptValue(eng, false);
	return QScriptValue(eng, strOutput);
}

///Function:Dialog.input
///Arguments:string strTitle, string strPrompt, string strDefaultText=""
///Returns:string strResult
///Doc:Opens input dialog for the user to type a response. Returns '' if user presses cancel.
///Implementation:c++_winext
{
	CHECK_ARGS
	QStringList args; 
	args << "text"  << strTitle <<  strPrompt << strDefaultText;
	QString strOutput = util_externalCmdStdout(G_WinCommonDialog, args);
	
	// if user presses cancel, prints <cancel>. Unfortunately, we can't really handle that, because it is caught in util_externalCmdStdout.
	
	return QScriptValue(eng, strOutput);
}

