#include <QtScript>
#include "autoit/AutoIt3.h"
#include "util_au3.h"

namespace launchorz_functions
{
	void util_au3init()
	{
		AU3_Init();
	}
	
	void util_SetAU3PixelRelativeCoords(bool bRelative)
	{
		if (bRelative) AU3_AutoItSetOption("PixelCoordMode",0);
		else AU3_AutoItSetOption("PixelCoordMode",1);
	}
	void util_SetAU3MouseRelativeCoords(bool bRelative)
	{
		if (bRelative) AU3_AutoItSetOption("MouseCoordMode",0);
		else AU3_AutoItSetOption("MouseCoordMode",1); //relative coords to the active window, not the client area 
	}
	void util_SetAU3CaretRelativeCoords(bool bRelative)
	{
		if (bRelative) AU3_AutoItSetOption("CaretCoordMode",0);
		else AU3_AutoItSetOption("CaretCoordMode",1); //relative coords to the active window, not the client area 
	}
	
	QString util_AU3KeyboardCommandsReplacement(QString strKeySequence)
	{
		strKeySequence.replace("+","{+}",Qt::CaseSensitive); strKeySequence.replace("!","{!}",Qt::CaseSensitive); strKeySequence.replace("#","{#}",Qt::CaseSensitive); strKeySequence.replace("^","{^}",Qt::CaseSensitive);
		strKeySequence.replace("<Alt>","!",Qt::CaseInsensitive); strKeySequence.replace("<Shift>","+",Qt::CaseInsensitive); strKeySequence.replace("<Control>","^",Qt::CaseInsensitive); strKeySequence.replace("<Cntrl>","^",Qt::CaseInsensitive); strKeySequence.replace("<Win>","#",Qt::CaseInsensitive);
		return strKeySequence;
	}
	QScriptValue util_QListToScriptArray(QScriptEngine *eng, QList<QString>& results)
	{
		QScriptValue ar = eng->newArray(results.count());
		for (int i=0; i<results.count(); i++)
			ar.setProperty(i, QScriptValue(eng, results[i]));
		return ar;
	}
	
	
	
	
	
	// previous to calling this, use.setFilter to set a filter.
	QScriptValue util_ListDirectoryEntries(QScriptContext *ctx, QScriptEngine *eng, QDir objDir, QString strSortBy)
	{
		//get the sorting
		QString strSortKey = strSortBy.toLower(); //normalize by case
		QFlags<QDir::SortFlag> flags = 0;
		if (strSortKey=="unsorted") flags = QDir::Unsorted; 
		else if (strSortKey=="name") flags = QDir::Name; 
		else if (strSortKey=="time") flags = QDir::Time; 
		else if (strSortKey=="size") flags = QDir::Size | QDir::Reversed;  //by default, sorts from largest down. We reverse this to make it more consistent (imo)
		else if (strSortKey=="type") flags = QDir::Type; 
		else return ctx->throwError("File.dirListFiles(). Unsupported sort flag. Use one of unsorted/name/time/size/type");
		
		objDir.setSorting(flags);
		QFileInfoList list = objDir.entryInfoList();
		QScriptValue ar = eng->newArray(list.size());
		// It would be more efficient if we used the fileInfo object to also return the information, since it was computed here anyways...
		for (int i = 0; i < list.size(); i++)
		{
			QFileInfo fileInfo = list.at(i);
			ar.setProperty(i, QScriptValue(eng, fileInfo.fileName()));
		}
		return ar;
	}
	
	
	
	QString util_nircmd_directory, util_wincommondlg_directory;
	void util_nircmd_init()
	{
		if (QFileInfo("nircmd.exe").exists())
		{
			// get current absolute path, and remember it.
			util_nircmd_directory = QFileInfo(".").absoluteFilePath().replace("/","\\");
			if (!util_nircmd_directory.endsWith("\\")) util_nircmd_directory+="\\";
			util_wincommondlg_directory = (util_nircmd_directory+"");
			util_nircmd_directory += "nircmd.exe";
			util_wincommondlg_directory += "WinCommonDialog.exe";
		}
		else 
		{
			util_nircmd_directory = ""; // it does not exist., we'll throw an error if we need it
			util_wincommondlg_directory = ""; 
		}
	}
	
	// kind of weird to have reference parameters having default arguments, but this apparently implicitly creates a QString of length 0, which can be tested with str==0.
	QScriptValue util_externalCmd(int program, QScriptContext *ctx, QScriptEngine *eng,  const QString& strCommand, const QString& arg1 /* =0*/, const QString& arg2, const QString& arg3, const QString& arg4, const QString& arg5, const QString& arg6)
	{
		if (util_nircmd_directory=="") return ctx->throwError("The file nircmd.exe could not be found. Place it in a location that can be found by LnzScript in order to use this function.");
		QString strExecutable;
		if (program == G_Nircmd) strExecutable = util_nircmd_directory + " ";
		else if (program == G_WinCommonDialog) strExecutable = util_wincommondlg_directory + " ";
		else return ctx->throwError("Internal error. Bad external command.");
		
		strExecutable += strCommand; // note unescaped! So, if everything is preformatted you can put it here - but make sure no unescaped quotes
		if (arg1!=0) strExecutable += " \""+util_external_escape(arg1)+"\""; //replace " with \", to escape quotes
		if (arg2!=0) strExecutable += " \""+util_external_escape(arg2)+"\"";
		if (arg3!=0) strExecutable += " \""+util_external_escape(arg3)+"\"";
		if (arg4!=0) strExecutable += " \""+util_external_escape(arg4)+"\"";
		if (arg5!=0) strExecutable += " \""+util_external_escape(arg5)+"\"";
		if (arg6!=0) strExecutable += " \""+util_external_escape(arg6)+"\"";
		
		// now do a "run and wait" (synchronous)
		long nStatus = AU3_RunWait(QStrToCStr(strExecutable), "",1); //default working directory, flag of 1
		if (program == G_Nircmd)
		{
			return (nStatus==0) ? QScriptValue(eng, true) : QScriptValue(eng, false); // "NirCmd now returns a non-zero value on error."
		}
		else if (program == G_WinCommonDialog)
		{
			return QScriptValue(eng, nStatus); // What is nice is that WinCommonDialog actually returns its result through the return code. Nice.
		}
	}
	
	
/*
	There are 5 different ways to pass in a way to specify a window.
	'Untitled'			Match starting with Untitled

	'"Untitled - Notepad"'		Match exact string

	'%Untitled%'			Match windows with part of string

	''				Empty string - currently active window

	{'title':'hello', 'class':'Notepad', 'instance':1}		Set all of these parameters
	
	for a title starting and ending with quotes, use {'title':'"literal quotes"'}
*/
	QString util_Au3WindowGetTextParamFromObj(QScriptValue& val)
	{
		if (val.isObject() && !val.isFunction())
		{
			QScriptValueIterator iter(val);
			while (iter.hasNext())
			{
				iter.next();
				QString strName = iter.name();
				if (QString::compare(strName, "TEXT", Qt::CaseInsensitive)==0)
					return iter.value().toString();
			}
		}
		return QString("");
	}
	
	QString util_Au3WindowParseAndSetMode(QScriptValue& val)
	{
		if (val.isString())
		{
			QString str = val.toString();
			if (str == "")						// Match currently active window
			{
				AU3_AutoItSetOption("WinTitleMatchMode",4);
				return QString("[ACTIVE]");
			}
			else if (str.startsWith('"') && str.endsWith('"'))	// Match exact string
			{
				AU3_AutoItSetOption("WinTitleMatchMode",3);
				return str.mid(1, str.length()-2); //strip off "
			}
			else if (str.startsWith('%') && str.endsWith('%'))	// Match as substring
			{
				AU3_AutoItSetOption("WinTitleMatchMode",2);
				return str.mid(1, str.length()-2); //strip off %
			}
			else		//normal match - Matches partial titles from the start.
			{
				AU3_AutoItSetOption("WinTitleMatchMode",1);
				return str;
			}
		}
		else if (val.isObject() && !val.isFunction())
		{
			QScriptValueIterator iter(val);
			if (!(iter.hasNext()))
				return NULL; //empty object, {}, invalid
			
			QString strResult("[");
			while (iter.hasNext())
			{
				iter.next();
				
				QString strName = iter.name();
				if (QString::compare(strName, "TEXT", Qt::CaseInsensitive)==0) // This parameter will be handled by util_Au3WindowGetTextParamFromObj - don't include it here
					continue;
				
				else if (QString::compare(strName, "TITLE", Qt::CaseInsensitive)==0) strResult.append("TITLE");
				else if (QString::compare(strName, "CLASS", Qt::CaseInsensitive)==0) strResult.append("CLASS");
				else if (QString::compare(strName, "REGEXPTITLE", Qt::CaseInsensitive)==0) strResult.append("REGEXPTITLE");
				else if (QString::compare(strName, "REGEXPCLASS", Qt::CaseInsensitive)==0) strResult.append("REGEXPCLASS");
				else if (QString::compare(strName, "INSTANCE", Qt::CaseInsensitive)==0) strResult.append("INSTANCE");
				else if (QString::compare(strName, "HANDLE", Qt::CaseInsensitive)==0) strResult.append("HANDLE");
				//else return NULL;
				else strResult.append(strName); // be lenient, accept bad values
				
				strResult.append(":");
				strResult.append( iter.value().toString().replace(";",";;",Qt::CaseSensitive) );
				
				if (iter.hasNext())
					strResult.append(";");
			}
			strResult.append("]");
			
			// Set mode
			AU3_AutoItSetOption("WinTitleMatchMode",4);
			return strResult;
		}
		else return NULL;
	}
	
}
