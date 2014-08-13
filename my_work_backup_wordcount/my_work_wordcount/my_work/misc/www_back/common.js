var choice;
var AxVer = "2,0,0,16";
var AxVer1 = "0,8,6,0";
var webPageVersion = "1.3";
var webPageSubVersion = "1";
var DigSignName = "APPROTECH";
var g_viewXSize = 0;
var g_viewYSize = 0;
var browser_Netscape = false;
var browser_IE = false;
var browser_FireFox = false;
var keyword_Show = '';
var keyword_Hide = '';
var AxID = "VideoCtrl";
var AxRoomID = "VideoAxRoom";
var g_MaxSubmitLen = 190; // it will be over, increase 30
var AxUuid1="2A7EB0E1-7CF0-468B-B936-33091E67CEE3";  //this is for Java Web Page Maker to replace.
//var AxUuid="5E4CCBDC-F575-4BE5-B9C2-C87F562DD0F8";
var AxUuid="9BE31822-FDAD-461B-AD51-BE1D1C159921";//ALU
var LAry = {};
var LangKeyPrefix = "lang_";
var g_CHID = 0;
var CONTENT_PAGE = '';
var CONTENT_PAGE_LAST = ''; // for link error backup.
var CTRLARY = {};
var c_iniUrl = "/vb.htm?language=ie";
var g_httpOK = true;
var g_SubmitHttp = null;
var g_lockLink = false; // lock the link , can not access.
var g_CHPageList = "c_sccd.htm;sdigital.htm;aenable.htm;motion.htm;img.htm;imgtune.htm;main.htm;";
var g_badLinkList = "^bwcntl\\.htm;^image\\.htm;^update\\.htm;^nftphost\\.htm;^help\\.htm;^ndhcpsvr\\.htm;^nupnp\\.htm;^k_\\w*\\.htm;^p_\\w*\\.htm;^ptz_\\w*\\.htm;^faq\\.htm;^version\\.htm;^index\\.htm;^sccd\\.htm;^armio\\.htm;^svideo\\.htm;^tailpage\\.htm;^mcenter\\.htm;^lang\\.htm"
    .split(";");
var g_AdvMode = 0; // display advance menu. 1:on , 0:off
var g_lastPolicy = 0;
var WCH = null; // WebContentHttp
// set language name , must has order.
var g_langFullNameList = new Array("cn_us","en_us", "zh_cn", "zh_tw", "cs_cz", "nl_nl", "fi_fi", "fr_fr", "de_de", "it_it", "pl_pl", "pt_pt", "es_es", "sv_se", "hu_hu", "ro_ro", "tr_tr");
var g_langNameList;
var g_langName;
var g_sh_net = true;
var g_sh_pppoe = true;
var g_backList = new Array();
var g_fwdList = new Array();
var NO_STORAGE = 255; // 255
var ISNOSTORE = (g_defaultStorage == NO_STORAGE);
var g_isShowUpdate = ((g_oemFlag0 & 0x00000001) != 0);
var g_isShowBWCtrl = ((g_oemFlag0 & 0x00000002) != 0);
var g_isShowUPnP = ((g_oemFlag0 & 0x00000004) != 0);
var g_tableWidth = 1380;
var g_isStartPtz;

// Multi Profile Case 1
// define the 7228 and 7227, width and hight
// define codec:
var V_JPEG = 1000;
var V_MPEG4 = 1005;
var V_H264 = 1010;

// keep the left top point
var baseX = 0;
var baseY = 0;

// 20080712 Luther , good string appender
StringBuffer = function()
{
  var col = [];
  this.Add = function(s)
  {
    col.push(s);
  };
  this.ToString = function()
  {
    return col.join("");
  };
};

// initialize language name.
{
  var o = '';
  var t = g_s_mui;
  var i = 0;
  while (t > 0)
  {
    if (t % 2 == 1)
    {
      o += g_langFullNameList[i] + ";";
    }
    t = Math.floor(t / 2);
    i++;
  }
  o = o.substring(0, o.length - 1);
  g_langNameList = o.split(";");
  if (g_langNameList.length == 0 || g_mui < 0 || g_langNameList[0] == "")
    g_langName = "en_us";
  else if (g_langNameList.length == 1)
    g_langName = g_langNameList[0];
  else
    g_langName = g_langFullNameList[g_mui];
};
g_langName="cn_us";
// This is xml http request object for dynamic fetch html
// var xhttp = null;
var THIS_PAGE = GetWebPageName(location.href);

// check brower type, and set show ,hide keyword.
if (window.ActiveXObject)
{
  browser_IE = true;
  keyword_Show = "visible";
  keyword_Hide = "hidden";
}
else if (document.layers)
{
  browser_Netscape = true;
  keyword_Show = "show";
  keyword_Hide = "hide";
}
else
{
  browser_FireFox = true;
  keyword_Show = "visible";
  keyword_Hide = "hidden";
}
var c_VideoMode = parseInt(GetCookie("VideoMode"));
g_isSupMpeg4 = (c_VideoMode == 1);

function FixViewSize()
{
  //set g_viewX(Y)Size
  if (IsMpeg4())
  {
    g_viewXSize = g_mpeg4XSize;
    g_viewYSize = g_mpeg4YSize;
  }
  else
  {
    g_viewXSize = g_jpegXSize;
    g_viewYSize = g_jpegYSize;
  }

  //Video Server fix size.
  if (IsVS())
  {
    g_viewXSize = 320;
    if (Is2114())
    {
      g_viewYSize = (g_isPal) ? 272 : 224;
    }
    else
    {
      g_viewYSize = (g_isPal) ? 288 : 240;
    }
  }
  SafeCall("UpdateViewSize");
};

FixViewSize();

// init the sub menu value
var MENU_ITEM_IMAGE;
var MENU_ITEM_SYSTEM;
var MENU_ITEM_NETWORK;
var MENU_ITEM_APP_SET;
var MENU_ITEM_APP_REC;
var MENU_ITEM_APP_ALARM;
var MENU_ITEM_EVENT;
ReloadSubMenu();

// ==================================
// Luther add String trim function
// ==================================
String.prototype.trim = function()
{
  return this.replace(/(^\s*)|(\s*$)/g, "");
}
String.prototype.ltrim = function()
{
  return this.replace(/(^\s*)/g, "");
}
String.prototype.rtrim = function()
{
  return this.replace(/(\s*$)/g, "");
}

function IsMozilla()
{
  return (browser_Netscape || browser_FireFox);
};

function IsInBadLinkList(link)
{
  if (link == null || link == "")
    return false;
  link = link.toLowerCase();
  for ( var i = 0; i < g_badLinkList.length; i++)
  {
    if (TestRE(link, g_badLinkList[i]))
    {
      return true;
    }
  }
  return false;
};

function TagAX1(id, width, height)
{
  var o = '<div id="' + AxRoomID + '" name="' + AxRoomID + '">';
  o+='<table width="800" height="600" bordercolor="#FF9900" style="margin-top:50px; margin-left:0px;" border="1">';
  o+='<tr>';
  o+='<td height="23" bgcolor="#006699"><font color="#FFFFFF">视频浏览</font></td>';
  o+='</tr>';
  o+='<tr>';
  o+='<td><table align="center" bgcolor="#000000" style="margin-top:20px" border="0">';
  o+='<tr><td height="40" border="0"></tr></td>';
  o+='<tr><td>';
  o+='<OBJECT " ID="' + ((id == null) ? AxID : (AxID + id)) + '"';
  o+=' CLASSID="CLSID:' + AxUuid + '"';
  o+=' CODEBASE="http://downloads.videolan.org/pub/videolan/vlc/0.8.6/win32/axvlc.cab#version=' + AxVer1 + '" width=' + width*0.8 + ' height=' + height*0.8 + ' >';
  o+='</OBJECT>';
  o+='</td></tr>';
  o+='<tr><td height="40" border="0"></tr></td>';
  o+='</table></td></tr>';
  o+='</table>';
  o+='</div>';
  return o;
};
function TagAX2(id, width, height)
{
  var o = '<div id="' + AxRoomID + '" name="' + AxRoomID + '"><table bordercolor="#FF9900" style="margin-top:50px;" border=1 ><tr><td><OBJECT " ID="' + ((id == null) ? AxID : (AxID + id)) + '"';
  o += ' CLASSID="CLSID:' + AxUuid1 + '"';
  o += ' CODEBASE="http://downloads.videolan.org/pub/videolan/vlc/0.8.6/win32/axvlc.cabb#version=' + AxVer1 + '" width=' + width + ' height=' + height + ' ></OBJECT></td></tr></table></div>';
  return o;
};

function ChangeAx2Pic(w, h, forceWait)
{
  var rooms = GES(AxRoomID);
  if (rooms != null)
  {
    if (w == null)
    {
      if (rooms.length > 1 || CONTENT_PAGE != "main.htm")
      {
        w = 320;
        h = 240;
      }
      else
      {
        w = 640;
        h = 480;
      }
    }
    var o = '<img src="noActiveX.gif" ';
    o += ' width="' + w + '" height="' + h + '"';
    o += ' />';

    for ( var i = 0; i < rooms.length; i++)
    {
      if (forceWait != true)
      {
        rooms[i].innerHTML = o;
      }
    }

  }
};

function GetLiveUrlNoID(ifrmonly, audioEn)
{
  var o;
  o = 'http://' + location.host + '/ipcam/' + (IsMpeg4() ? "mpeg4" : "mjpeg") + '.cgi?language=ie';

  if (ifrmonly == 1)
  {
    o += "&ifrmonly=1";
  }
  if (audioEn == 1)
  {
    o += "&audiostream=1";
  }
  return o;
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////\

function StartActiveXOneEx(ifrmonly, audioEnable, codec, chid, axid, mode, forceWait)
{

 var obj = (axid == null) ? GE(AxID) : GE(AxID + axid);
 var targetURL;
  if (obj != null)
  {
    try
    {
     var URL=location.host;
     if(codec==1010)
       //targetURL='rtsp://192.168.2.187:8557/h264';
        targetURL='rtsp://'+URL+':8557/h264';
     else if(codec==1005)
         targetURL='rtsp://'+URL+':8554/mpeg4';
     else if(codec==1000) 
        targetURL='rtsp://'+URL+':8556/mjpeg'; 
      //targetURL='rtsp://192.168.1.168:8557/h264';
        obj.playlist.clear();
      obj.playlist.add(targetURL);
       obj.playlist.play();
    }
    catch (e)
    {
   ChangeAx2Pic(null, null, forceWait);
    }
  }
  else
  {
      ChangeAx2Pic();
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// forceWait: do not show the graph message. it will wait the ActiveX be installed.
// mode:99 is for display title
/*
function StartActiveXOneEx(ifrmonly, audioEnable, codec, chid, axid, mode, forceWait)
{
  var obj = (axid == null) ? GE(AxID) : GE(AxID + axid);

  if (obj != null)
  {
    try
    {
	  var v_ReqCommand = "/ipcam/";
	  obj.Stop();
	  obj.URL = location.host;
	  obj.User = "";
	  obj.Password = "";
	  if(codec == 1000)
			v_ReqCommand = v_ReqCommand + "mjpeg.cgi";
	  else if(codec == 1010)
	  {
			if(chid==6)
				v_ReqCommand = v_ReqCommand + "avccif.cgi";
			else 	v_ReqCommand = v_ReqCommand + "avc.cgi";
	  }
	  else if(codec == 1005)
	  {
			if(chid==3)
				v_ReqCommand = v_ReqCommand + "mpeg4cif.cgi";
			else	v_ReqCommand = v_ReqCommand + "mpeg4.cgi";
	  }
	  obj.ReqCommand =v_ReqCommand;
	  if(location.port)
		obj.HttpPort = location.port;
	  else
		obj.HttpPort = 80;
	  obj.Width = g_viewXSize;
	  obj.Height = g_viewYSize;

	  //Status 0 is none, 1 is Privacy Mask , 2 is motion ,3 is Zoom
	  if(mode==3)
	  {
		obj.Status(2);
	  }
	  else
	  {
	  obj.Status(0);
	  }
	  //mSamplesPerSec for ulaw
	  obj.AudioSamplesPerSec = 8000;

	  //if motion
	  obj.Motionxblock = g_motionxblock;
	  obj.Motionyblock = g_motionyblock;
	  obj.MotionBlock = g_motionblock;
	  //alert("v_ReqCommand:"+v_ReqCommand+",location.host:"+location.host+",g_viewXSize:"+g_viewXSize+",g_viewYSize:"+g_viewYSize+",mode:"+mode);
	  obj.Start();
    }
    catch (e)
    {
      ChangeAx2Pic(null, null, forceWait);
    }
  }
  else
  {
    ChangeAx2Pic();
  }
};
*/
// id : channel id.
// forceWait: do not show the graph message. it will wait the ActiveX be installed.
// codec: prefer show which codec?
function StartActiveXEx(ifrmonly, audioEnable, codec, chid, axid, mode, forceWait)
{
   
  var myID = null;
  if (IsVS())
  {
    // id == null mean run all.
    if (chid == null || chid == 0)
    {
      var i;
      for (i = 1; i <= g_maxCH; i++)
      {
        StartActiveXOneEx(ifrmonly, audioEnable, codec, i, i, mode, forceWait);
      }
    }
    else
    {
      StartActiveXOneEx(ifrmonly, audioEnable, codec, chid, axid, mode, forceWait);
    }
  }
  else
  {
    //ipcam has no channel
    StartActiveXOneEx(ifrmonly, audioEnable, codec, chid, null, mode, forceWait);
  }

};
// get object from id
function GE(name)
{
  return document.getElementById(name);
};
// get obj array from name
function GES(name)
{
  return document.getElementsByName(name);
};
// genernate the "select" html tag
function SelectObject(strName, strOption, intValue, onChange)
{
  DW(SelectObjectNoWrite(strName, strOption, intValue));
};
function SelectObjectNoWrite(strName, strOption, intValue, onChange)
{
  var o = '';
  o += '<SELECT NAME="' + strName + '" id="' + strName + '" class="m1"';
  if (onChange == null)
  {
    o += '>';
  }
  else
  {
    o += ' onChange="' + onChange + '" >';
  }
  aryOption = strOption.split(';');
  for ( var i = 0; i < aryOption.length; i++)
  {
    if (i == intValue)
    {
      o += '<OPTION selected value=' + i + '>' + aryOption[i];
    }
    else
    {
      o += '<OPTION value=' + i + '>' + aryOption[i];
    }
  }
  o += '</SELECT>';
  return o;
};

// Genernate "select" , the value is number
// include start and end
function CreateSelectNumber(name, start, end, gap, init)
{
  DW(GetSelectNumberHtml(name, start, end, gap, init));
};
// fixNum : the digital fix count.
function GetSelectNumberHtml(name, start, end, gap, init, onChange, onFocus, fixNumC)
{
  var o = '';
  o += '<select id="' + name + '" name="' + name + '" class="m1" ';
  if (onChange != null)
  {
    o += ' onchange="' + onChange + '"';
  }
  if (onFocus != null)
  {
    o += ' onfocus="' + onFocus + '"';
  }
  o += '>';
  var i = 0;
  for (i = start; i <= end; i += gap)
  {
    var fixStr = i;
    if (fixNumC != null)
    {
      fixStr = FixNum(i, fixNumC);
    }
    if (i == init)
    {
      o += '<option selected value=' + i + '>' + fixStr;
    }
    else
    {
      o += '<option value=' + i + '>' + fixStr;
    }
  }
  o += '</select>';
  return o;
};

// return the radio button that be checked.
function GetRadioValue(name)
{
  var value = 0;
  var i;
  var radioObj = GES(name);
  if (radioObj != null)
  {
    for (i = 0; i < radioObj.length; i++)
    {
      if (radioObj[i].checked == true)
      {
        value = radioObj[i].value;
        break;
      }
    }
  }
  return value;
};
// if radio button value equal vv , then check this option.
function SetRadioValue(name, vv)
{
  var i;
  var radioObj = GES(name);
  if (radioObj != null)
  {
    for (i = 0; i < radioObj.length; i++)
    {
      if (radioObj[i].value == vv)
      {
        radioObj[i].checked = true;
        break;
      }
    }
  }
};
function IsMpeg4()
{
  return ((g_isSupMpeg4 == "1") && (g_videoFormat == "1"));
};

// is mulit profile?
// H264 or support MP
function IsMP()
{
  return (g_isSupportMultiProfile || g_isMP1 || g_isMP73);
};

function CreateText(name, size, maxlength, value, isPassword, onChangeFunc)
{
  DW(CreateTextHtml(name, size, maxlength, value, isPassword, onChangeFunc));
};
function CreateTextHtml(name, size, maxlength, value, isPassword, onChangeFunc, onKeyUP)
{
  var type = "text";
  if (isPassword)
  {
    type = "password";
  }
  var o = '';
  o += "<input name='" + name + "' id='" + name + "' type='" + type + "' size='" + size + "' class='m1' maxlength='" + maxlength + "' value='" + value + "'";
  if (onChangeFunc != null)
  {
    o += "onChange='" + onChangeFunc + "' ";
  }
  if (onKeyUP != null)
  {
    o += "onKeyup='" + onKeyUP + "' ";
  }
  o += ">";
  return o;
};

// ===================================================
// Validate
// ===================================================
// test the reqular expression , v is value, re is regualar expression
function TestRE(v, re)
{
  return new RegExp(re).test(v);
};

// check the string is null or blank.
// return ture -> str is not null or ""
// return false -> str is
function CheckIsNull(str, defMsg, msg, isQuite)
{
  var result = false;
  if (CheckIsNullNoMsg(str))
  {
    if (isQuite != true)
    {
      if (msg != null)
      {
        alert(msg);
      }
      else
      {
        alert(GL("verr_vacuous",
        { 1 : defMsg }));
      }
    }
    result = true;
  }
  return result;
};
function CheckIsNullNoMsg(str)
{
  return (str == null || str == "");
};
// ===================================================
// check E-Mail
function CheckBadEMail(str, msg, isQuite)
{
  var result = false;
  if (!TestRE(str, '^[-!#$%&\'*+\\./0-9=?A-Z^_`a-z{|}~]+@[-!#$%&\'*+\\/0-9=?A-Z^_`a-z{|}~]+\.[-!#$%&\'*+\\./0-9=?A-Z^_`a-z{|}~]+$'))
  {
    if (isQuite != true)
    {
      if (msg == null)
      {
        alert(GL("verr_email"));
      }
      else
      {
        alert(msg);
      }
    }
    result = true;
  }
  return result;
};

function IsNumeric(sText)
{
  var ValidChars = "0123456789";
  var IsNumber = true;
  var i;
  var Char;
  for (i = 0; i < sText.length && IsNumber == true; i++)
  {
    Char = sText.charAt(i);
    if (ValidChars.indexOf(Char) == -1)
    {
      IsNumber = false;
    }
  }
  return IsNumber;
}

function CheckBadFQDN(str, msg, isQuite)
{
  var result = false;
  //var strRE = "^\w([\w\-]+\.)+\w+$";
  rObj = new RegExp("^\\w([\\w-]+\\.)+\\w+$");
  if (!str.match(rObj)) {
    if (isQuite != true) {
      if (msg == null) {
        alert("URL/Address is not correct.");
      } else {
        alert(msg);
      }
    }
    result = true;
  }
  return result;
};

// ===================================================
// check the range of number.

// check the range of number ,include min and max ( min <= x <= max)
// true: the number(str) is not in the range.
function CheckBadNumberRange(str, min, max, defMsg, msg, isQuite)
{
  var result = false;
  var num = parseInt(str);
  if (!((num >= min && num <= max) && (IsNumeric(str))))
  {
    if (isQuite != true)
    {
      if (msg == null)
      {
        alert(GL("verr_bad_num",
        { 1 : defMsg, 2 : min, 3 : max }));
      }
      else
      {
        alert(msg);
      }
    }
    result = true;
  }
  return result;
};
// ===================================================
// check the length of the string.
// true: if the length of the string is not in range(minLen <= x <= maxLen)
function CheckBadStrLen(str, minLen, maxLen, defMsg, msg, isQuite)
{
  var result = false;
  if (str == null)
  {
    result = true;
  }
  else
  {
    var len = str.length;
    if (!(len >= minLen && len <= maxLen))
    {
      if (isQuite != true)
      {
        if (msg == null)
        {
          if (minLen != maxLen)
          {
            alert(GL("verr_str_len",
            { 1 : defMsg, 2 : minLen, 3 : maxLen }));
          }
          else
          {
            alert(GL("verr_str_len2",
            { 1 : defMsg, 2 : minLen }));
          }
        }
        else
        {
          alert(msg);
        }
      }
      result = true;
    }
  }
  return result;
};

// check string , include the non alphabet or digital char.
// true : found non alphabet or digital char, False: all chars are belong alphabet or digital number
// noCheckNum : do not check digital number. it's mean only check alphabet
// noCheckLower: do not check lower alphabet,
// noCheckLower: do not check upper alphabet,
function CheckBadEnglishAndNumber(str, defMsg, msg, noCheckNum, noCheckLower, noCheckUpper, isQuite)
{
  var result = false;
  if (str == null)
  {
    result = true;
  }
  else
  {
    var i = 0;
    for (i = 0; i < str.length; i++)
    {
      var cc = str.charCodeAt(i);
      var checker = true; // true : means baddly.
      if (noCheckNum == null || noCheckNum == false)
      {
        checker = checker && !(cc >= 48 && cc <= 57);
      }
      if (noCheckLower == null || noCheckLower == false)
      {
        checker = checker && !(cc >= 97 && cc <= 122);
      }
      if (noCheckUpper == null || noCheckUpper == false)
      {
        checker = checker && !(cc >= 65 && cc <= 90);
      }
      if (checker)
      {
        if (isQuite != true)
        {
          if (msg == null)
          {
            alert(GL("verr_eng_digital",
            { 1 : defMsg }));
          }
          else
          {
            alert(msg);
          }
        }
        result = true;
        break;
      }
    }
  }
  return result;
};
// check string , include the non alphabet or digital char.
// true : found non alphabet or digital char, False: all chars are belong alphabet or digital number
// noCheckNum : do not check digital number. it's mean only check alphabet
// noCheckLower: do not check lower alphabet,
// noCheckLower: do not check upper alphabet,
function CheckFTPPath(str, defMsg, msg, noCheckNum, noCheckLower, noCheckUpper, isQuite)
{
  var result = false;
  if (str == null)
  {
    result = true;
  }
  else
  {
    var i = 0;
    for (i = 0; i < str.length; i++)
    {
      var cc = str.charCodeAt(i);
      var checker = true; // true : means baddly.
      if (noCheckNum == null || noCheckNum == false)
      {
        checker = checker && !(cc >= 47 && cc <= 57);
      }
      if (noCheckLower == null || noCheckLower == false)
      {
        checker = checker && !(cc >= 97 && cc <= 122)&&!(cc == 95);
      }
      if (noCheckUpper == null || noCheckUpper == false)
      {
        checker = checker && !(cc >= 65 && cc <= 90);
      }
      if (checker)
      {
        if (isQuite != true)
        {
          if (msg == null)
          {
            alert(GL("verr_ftp_eng_dig",
            { 1 : defMsg }));
          }
          else
          {
            alert(msg);
          }
        }
        result = true;
        break;
      }
    }
  }
  return result;
};

function DW(str)
{
  document.write(str);
};
// ===================================================
// Submit the form
// ===================================================
// create the submit button
// CLID: Control List ID
function CreateSubmitButton(CLID, isAsync)
{
  DW(CreateSubmitButton_(CLID, isAsync));
};
function CreateSubmitButton_(CLID, isAsync)
{

  var fun = "ValidateCtrlAndSubmit";
  fun += ((CheckIsNullNoMsg(CLID)) ? "(CTRLARY" : ("(" + CLID));
  fun += ((CheckIsNullNoMsg(isAsync)) ? "" : ("," + isAsync));
  fun += ")";
  // alert(fun+isAsync);
  var o = '';
  o += '<tr align="right"><td colspan=4><input type="button" style="width=60;height=30;font-size:medium" id="smbtn_' + ((CLID == null) ? "" : CLID) + '" value="' + GL("submit") + '" class="m1" onClick="' + fun + '"></td></tr>';
  return o;

};

function SendHttp(url, isAsync, callBack)
{
  isAsync = new Boolean(isAsync);
  g_SubmitHttp = null;
  g_SubmitHttp = InitXHttp();
  if (callBack != null)
  {
    g_SubmitHttp.onreadystatechange = callBack;
  }
  else
  {
    g_SubmitHttp.onreadystatechange = OnSubmitReadyStateProcess;
  }

  try
  {
    g_SubmitHttp.open("GET", url, isAsync);
    g_SubmitHttp.setRequestHeader("If-Modified-Since", "0");
    g_SubmitHttp.send(null);
    WS(GL("sending_"));
  }
  catch (e)
  {};

};

function OnSubmitReadyStateProcess()
{
  if (g_SubmitHttp.readyState == 4)
  {
    if (g_SubmitHttp.status != 200)
    {
      alert(GL("err_submit_fail"));
      g_httpOK = false;
      WS(GL("fail_"));
    }
    else
    {
      g_httpOK = true;
      WS(GL("ok_"));
    }
  }
};

function ShowObjVar(obj)
{
  var ssss;
  for (i in obj)
    ssss += i + "=" + obj[i] + ", ";
  document.write("<h1>" + ssss + "</h1>");
};

function IsChecked(name)
{
  var obj = GE(name);
  var result = false;
  if (obj != null)
  {
    result = obj.checked;
  }
  return result;
};
function SetChecked(name, isChk)
{
  var obj = GE(name);
  if (obj != null)
  {
    obj.checked = isChk;
  }
};
function Bool2Int(data)
{
  return (data) ? 1 : 0;
};
// ===================================================
// Get the value form the object.
// ===================================================
// return obj.value
// noErr: true do not show error message.
function GetValue(name, noErr)
{
  var result = '';
  var obj = GE(name);
  if (obj != null)
  {
    if (obj.type.indexOf("select") >= 0)
    {
      var ix = obj.selectedIndex;
      if (ix < 0)
      {
        result = -1;
      }
      else
      {
        result = obj.options[ix].value;
      }
    }
    else
    {
      result = obj.value;
    }
  }
  else if (noErr != true)
  {
    alert(GL("verr_miss_obj",
    { 1 : name }));
  }
  return result;
};

// ===================================================
// set the value of the object.
// ===================================================
function SetValue(name, value)
{
  var obj = GE(name);
  if (obj != null)
  {
    if (obj.type.indexOf("select") >= 0)
    {
      for ( var i = 0; i < obj.options.length; i++)
      {
        if (obj.options[i].value == value)
        {
          obj.selectedIndex = i;
          break;
        }
      }
    }
    else
    {
      obj.value = value;
    }
  }
  else
  {
    alert(GL("verr_set_value",
    { 1 : name }));
  }
};

function GetDeviceTitleStr(extName)
{
  var devName = g_deviceName;

  if (extName == null)
  {
    return GetTitleTag(devName);
  }
  else
  {
    return GetTitleTag(devName + extName);
  }
};
function GetTitleTag(titleStr)
{
  return ("<title>" + titleStr + "</title>");
};
// if exist otherName then the document title will be "otherName" and the "extTitle" will be nonsense
// if the "otherName" = null, but the "extTitle" exist, then the document title will be "<%devicename%> + extTitle"
// If both of them are equals null, then the document title will be "<%devicename%>" , and the "<%devicename%>" default value is "LAN Camera"
function WriteHtmlHead(extTitle, otherName, onLoadFunc, onUnLoadFunc, onResizeFunc, refreshTime)
{
  var o = '';
  o += GetHtmlHeaderNoBannerStr(extTitle, otherName, onLoadFunc, onUnLoadFunc, onResizeFunc, refreshTime);
  o += '<tr><td colSpan=2 style="padding-left:0px;" width=1500px >';
  o += GetLogoHtml();
  o += GetViewCHHtml();
  //o += '<img id="arrowImg" style="position:absolute;left:-600px;z-index:5;cursor: pointer;" src="arrow.gif" onClick="ClickArrow(event)" onMouseMove="MoveOnArrow//(event)" onMouseOut="WS()" border=0 />';
  //fy add
 o += '<div id ="imgbrand" style="background-image: url(newbrand2.gif); width:1500; height:100; "  useMap=#Map border=0> </div>';
 o += '<div id="LOGO_TI" style="position:absolute; left:0px; top:0px; width:150px; height:66px; z-index:10;"><img id="LOGO_PIC_TI" src="new_log.gif" style="cursor:pointer" alt="" onMouseOver="WS();" onMouseLeave="WS();"></div>';
 o += '</td></tr>';
 o += '<tr><td style="padding-left: 0px;" width="160px" height="700px" valign="top" background="ie_01.jpg">&nbsp;</td>';
 o += '<td valign="top"  style="margin-left:160px;" width=1340px ><div id="WebContent" >';
  DW(o);

};

function GetHtmlHeaderNoBannerStr(extTitle, otherName, onLoadFunc, onUnLoadFunc, onResizeFunc, refreshTime)
{
  var o = '';
  o += GetHtmlHeaderNoBodyStr(extTitle, otherName, refreshTime);

  o += '<body  leftmargin="0" topmargin="0" marginwidth="0" marginheight="0" onload = "initMenu()"';
  if (onLoadFunc != null)
  {
    o += 'onLoad="' + onLoadFunc + '()" ';
  }
  if (onUnLoadFunc != null)
  {
    o += 'onUnLoad="' + onUnLoadFunc + '()" ';
  }
  if (onResizeFunc != null)
  {
    o += 'onResize="' + onResizeFunc + '()" ';
  }
  o += '><table id = "theObjTable" cellSpacing=0 cellPadding=0 width=1500px border=0px align="left">';//3.15
  return o;
};

function GetHtmlHeaderNoBodyStr(extTitle, otherName, refreshTime)
{
  var o = '';
  if (otherName != null)
  {
    o += GetTitleTag(otherName);
  }
  else if (extTitle != null)
  {
    o += GetDeviceTitleStr(extTitle);
  }
  else
  {
    o += GetDeviceTitleStr();
  }
  if (refreshTime != null)
  {
    o += '<meta HTTP-EQUIV="Refresh" CONTENT="' + refreshTime + '">';
  }
  o += '<meta HTTP-EQUIV="CACHE-CONTROL" CONTENT="NO-CACHE">';
  o += '<link href="lc_en_us.csl" rel="stylesheet" type="text/css">';
  o += '</head>';
  return o;
};

function IsViewer()
{
  return (g_isAuthorityChange && g_socketAuthority > 1);
};
// get link string, it will depends on authority.
// 20060801 fix link to use XMLHttpRequest to fetch web page.
function GetLinkStr(link, key, type)
{
  var o = '';
  if (IsViewer())
  {
    o += GL(key);
  }
  else
  {
    o += GetContentLink(key, link, type);
  }
  return o;
};

function MainMenuIn(obj, key, isMM)
{
  obj.className = "menuLinkON";
  if (isMM != false)
  {
    WS(GL(key + "_comment"));
  }
};

function MainMenuOut(obj, key)
{
  obj.className = "menuLink";
  WS();
};

function GetFirstOKLink(menuAry)
{
  for ( var i = 2; i < menuAry.length; i += 3)
  {
    if (parseInt(menuAry[i]) == 1)
      return menuAry[i - 1];
  }
  return null;
};
//////////////////////////////////////fy add
//在ID为“js_menu”的div内开始程序
function initMenu()
{
      var pm_menu = new JSMenu("js_menu");
      pm_menu.init();
}
//定义主函数
function JSMenu(id) 
{
    if (!document.getElementById || !document.getElementsByTagName)
        return false;
    this.menu = document.getElementById(id);
    this.submenus = this.menu.getElementsByTagName("div");
}
//引入函数，取得所有span
JSMenu.prototype.init = function() 
{
    var mainInstance = this;
    for (var i = 0; i < this.submenus.length; i++)
        this.submenus[i].getElementsByTagName("span")[0].onclick = function() 
        {
            mainInstance.toggleMenu(this.parentNode);
        };
    this.expandOne();
};
//展开含"current"的菜单
JSMenu.prototype.expandOne = function() 
{
    for (var i = 0; i < this.submenus.length; i++) 
	{
        var links = this.submenus[i].getElementsByTagName("a");
        for (var j = 0; j < links.length; j++)
		{
            if (links[j].className == "current")
            this.expandMenu(this.submenus[i]);
        }
    }
};
//变换菜单状态函数
JSMenu.prototype.toggleMenu = function(submenu) 
{
    if (submenu.className == "collapsed")
        this.expandMenu(submenu);
    else
        this.collapseMenu(submenu);
};
//展开所有菜单函数
JSMenu.prototype.expandMenu = function(submenu) 
{
    var fullHeight = submenu.getElementsByTagName("span")[0].offsetHeight;
    var links = submenu.getElementsByTagName("a");
    for (var i = 0; i < links.length; i++)
        fullHeight += links[i].offsetHeight;
    var moveBy = Math.round(5 * links.length);
    
    var mainInstance = this;
    var intId = setInterval(function() 
	{
        var curHeight = submenu.offsetHeight;
        var newHeight = curHeight + moveBy;
        if (newHeight < fullHeight)
            submenu.style.height = newHeight + "px";
        else {
            clearInterval(intId);
            submenu.style.height = "";
            submenu.className = "";
        }
    }, 30);
    this.collapseOthers(submenu);
};
//折叠菜单函数
JSMenu.prototype.collapseMenu = function(submenu) {
    var minHeight = submenu.getElementsByTagName("span")[0].offsetHeight;
    var moveBy = Math.round(5 * submenu.getElementsByTagName("a").length);
    var mainInstance = this;
    var intId = setInterval(function() {
        var curHeight = submenu.offsetHeight;
        var newHeight = curHeight - moveBy;
        if (newHeight > minHeight)
            submenu.style.height = newHeight + "px";
        else {
            clearInterval(intId);
            submenu.style.height = "";
            submenu.className = "collapsed";
        }
    }, 30);
};
//折叠其他菜单函数
JSMenu.prototype.collapseOthers = function(submenu) {
        for (var i = 0; i < this.submenus.length; i++)
            if (this.submenus[i] != submenu && this.submenus[i].className != "collapsed")
                this.collapseMenu(this.submenus[i]);
};
/////////////////////////////////////
var totalMenuItem = 0;
function GetMenuStr()
{
  var o = '';
  o += '<table id ="tabmenu" cellSpacing=0 cellPadding=0 width="130" >';
 // o += '<tr><td width="40" height="24" background="bgline.gif">';
  //if (IsViewer())
 // {
  //  o += '&nbsp;';
 // }
 // else
 // {
 //   o += '<a href="version.htm" target=_blank>';
 //   o += '<img border="0" src="version.gif" width="10" height="10"></a>';
 // }
 //fy
  o += '</td><td class="td1" id="MI_0"><span class="menuLink" onMouseOver="MainMenuIn(this,\'\')" onMouseOut="MainMenuOut(this,\'\')" ><a href="/index.htm">现场</a></span></td>';
  o += '<td class="td1" id="MI_1">' + GetLinkStr("index.htm", "image") + '</td>';
  o += '<td class="td1" id="MI_2">' + GetLinkStr("net.htm", "network") + '</td>';
  o += '<td class="td1" id="MI_3">' + GetLinkStr("sdt.htm", "system") + '</td>';
  o += '<td class="td1" id="MI_7">' + GetLinkStr("ess.htm", "eventstorage")+'</td>';
  totalMenuItem++;
 // o += '<td width="40" background="bgline.gif">&nbsp;</td>
  o+='</tr></table>';
  return o;
};
function PopupPTZ(URL)
{
  SendHttp(c_iniUrl + GetSetterCmdKV("ptzpopup", "1"), false);
  return PopupPage(URL, "PTZ", 80, 0, 455, 125);
};
function PopupPage(URL, id, x, y, w, h)
{
  var windowProps = "location=no,scrollbars=no,menubars=no,toolbars=no,resizable=no,left=" + x + ",top=" + y + ",width=" + w + ",height=" + h;
  return WindowOpen(URL, id, windowProps);
};
function WindowOpen(URL, id, props)
{
  var popup = window.open(URL, id, props);
  try
  {
    popup.focus();
  }
  catch (e)
  {};
};

function GetMapLinkStr()
{
  var o = '';
  o += '<MAP name=Map>';
  o += '<AREA shape=RECT coords="556, 42, 636, 58" href="index.htm" name="index">';
  o += '<AREA shape=RECT coords="692, 42, 757, 58" href="logout.htm" name="logout">';
  o += '</MAP>';
  return o;
};

function WriteBottom(lastctx)
{
  var o = '';
  o += '</div></td>';
//  o += '<td width=14 valign="top" background="ie_04.jpg" height="492">&nbsp;</td></tr>';
    o += '</tr>';
  o += '<tr><td colSpan=3 id="mainMenuBar">';
//  o += GetMenuStr();
  o += '</td></tr>';
//  o += '<tr><td colSpan=3  background="address.jpg">';
//  o += '<p align="right" class="m2"><span id="devTitleLayer"></span>&nbsp&nbsp&nbsp&nbsp;</p>';
//  o += '</td></tr></table>';
o += '</table>';
  o += GetMapLinkStr();
  if (lastctx != null)
  {
    o += lastctx;
  }
  o += '</body>';
  DW(o);
  ResizeMenuItems();
};

function ResizeMenuItems()
{
  //resize
  var tmpW = 530 / totalMenuItem;
  for (i = 0; i < 9; i++)
  {
    try
    {
      var obj = GE("MI_" + i);
      if (obj != null)
      {
        obj.width = tmpW;
      }
    }
    catch (e)
    {};
  }
};

function ReloadSubMenu()
{
  MENU_ITEM_IMAGE = new Array('image', 'img.htm', 1, 'fine_tune', 'imgtune.htm', 1);
  MENU_ITEM_SYSTEM = new Array('date_and_time', 'sdt.htm', 1, 'timestamp', 'sts.htm', (g_supportTStamp > 0) ? 1 : 0, 'users', 'suser.htm', 1, 'audio_mechanism', 'saudio.htm', (g_isSupportAudio) ? 1 : 0, 'rs485_set', 'srs.htm', (g_isSupportRS485) ? 1 : 0, 'update', 'c_update.htm', (g_isShowUpdate) ? 1 : 0, 'event', 'aevt.htm', 1);
  MENU_ITEM_NETWORK = new Array("network1", "net.htm", 1, "nftp", "nftp.htm", g_serviceFtpClient, "nsmtp", "nsmtp.htm", g_serviceSmtpClient, "sntp", "nsntp.htm", g_serviceSNTPClient, "upnp", "c_nupnp.htm", (g_isShowUPnP) ? 1 : 0, "multicast", "multicast.htm", 1);
  MENU_ITEM_APP_SET = new Array("video_file", "setvid.htm", 1, "ftp", "setftp.htm", g_serviceFtpClient, "app_sd_card", "setcard.htm", (parseInt(g_defaultStorage) == 1) ? 1 : 0, "smtp", "setsmtp.htm", g_serviceSmtpClient);
  MENU_ITEM_APP_REC = new Array("enable_rec", "renable.htm", ((!ISNOSTORE) || (g_serviceFtpClient == 1)) ? 1 : 0, "schedule", "rsch.htm", ((!ISNOSTORE) || (g_serviceFtpClient == 1)) ? 1 : 0);

  MENU_ITEM_APP_ALARM = new Array("enable_alarm", "aenable.htm", 1, "motion_detect", "motion.htm", (g_isSupMotion) ? 1 : 0);
  MENU_ITEM_EVENT=new Array("event","ess.htm",1,"storage","storage.htm",1);

};

function ReloadMainMenu()
{
  GE("mainMenuBar").innerHTML = GetMenuStr();
  ResizeMenuItems();

};

// 20080428 Luther add ---START---
// use timer to check the ActiveX is alive?
// it will return true or false
function CheckActiveXAlive()
{
  var res = true;
  var obj = GE(AxID);
  if (obj != null)
  {
    try
    {
      //if (obj.IsAlive == 0)
      {
        res = false;
      }
    }
    catch (e)
    {}
  }
  return res;
}
//20080428 Luther add --- END ---

function StopActiveX()
{
  var obj = GE(AxID);
  if (obj != null)
  {
    try
    {
	  obj.Stop();
    }
    catch (e)
    {}
  }
  if (IsVS())
  {
    var i;
    for (i = 1; i <= g_maxCH; i++)
    {
      obj = GE(AxID + i);
      if (obj != null)
      {
        try
        {
          //obj.NotifyStop();
        }
        catch (e)
        {}
      }
    }
    for (i = 1; i <= g_maxCH; i++)
    {
      obj = GE(AxID + i);
      if (obj != null)
      {
        try
        {
          obj.Stop();
        }
        catch (e)
        {}
      }
    }
  }

};

function CheckHex(n, isQuite)
{
  var i, strTemp;
  strTemp = "0123456789ABCDEF";
  if (n.length == 0)
  {
    if (isQuite != true)
      alert(GL("verr_vacuous",
      { 1 : "hex" }));
    return false;
  }
  n = n.toUpperCase();
  for (i = 0; i < n.length; i++)
  {
    if (strTemp.indexOf(n.charAt(i)) == -1)
    {
      if (isQuite != true)
        alert(GL("verr_not_hex"));
      return false;
    }
  }
  return true;
};

function InitXHttp()
{
  var xhttp = null;
  if (IsMozilla())
  {
    xhttp = new XMLHttpRequest();
    if (xhttp.overrideMimeType)
    {
      xhttp.overrideMimeType('text/xml');
    }
  }
  else if (browser_IE)
  {
    try
    {
      xhttp = new ActiveXObject("Msxml2.XMLHTTP");
    }
    catch (e)
    {
      try
      {
        xhttp = new ActiveXObject("Microsoft.XMLHTTP");
      }
      catch (e)
      {};
    }
  }
  return xhttp;
};
function GetWebPageName(url)
{
  var ss = url.split("/");
  return ss[ss.length - 1];
};
function CreateSystemMenuItem(key, menuUrl, isON)
{
  if (isON == null)
    isON = 1;
  if (isON != 1)
    return '';
  if ((key == "ftp" || key == "smtp") && IsMpeg4())
    return '';

  var o = '';
  o += '<tr>';
  // srs.htm
  var isTitle = false;
  if (menuUrl.indexOf("srs.htm") == 0)
  {
    isTitle = (menuUrl == CONTENT_PAGE);
  }
  else if (CONTENT_PAGE.indexOf(menuUrl) == 0)
  {
    isTitle = true;
  }
  if (isTitle)
  {
    o += '<td width=22 height="22"><DIV align=center><img height=10 src="dot.jpg" width=10></DIV></td>';
    o += '<td width=128 height="22" class="sel">' + GL(key) + '</td>';
  }
  else
  {
    o += '<td width="22" height="22" >&nbsp;</td>';
    o += GetMenuItemLink(key, menuUrl);
  }
  o += '</tr>';
  return o;
};
function GetMenuItemLink(key, url, type)
{
  var o = "";
  o += '<td width="128" height="22" class="menuLink" onMouseOver="MainMenuIn(this,\'' + key + '\')" onMouseOut="MainMenuOut(this,\'' + key + '\')" onClick="';
  if (type == "pop")
  {
    o += 'PopupPTZ(\'' + url + '\');';
  }
  else if (type == "card")
  {
    o += 'WindowOpen(\'' + url + '\',\'CARD\',\'location=yes,directorybuttons=no,scrollbars=yes,resizable=yes,menubar=yes,toolbar=yes\');';
  }
  else
  {
    o += 'ChangeContent(\'' + url + '\');';
  }
  o += '" >' + GL(key) + '</td>';
  return o;

};
function GetContentLink(key, url, type)
{
  var o = "";
  o += '<span class="menuLink" onMouseOver="MainMenuIn(this,\'' + key + '\')" onMouseOut="MainMenuOut(this,\'' + key + '\')" onClick="';
  if (type == "pop")
  {
    o += 'PopupPTZ(\'' + url + '\');';
  }
  else if (type == "popHide")
  {
    o += 'PopupPage(\'' + url + '\',\'PTZHIDE\',10000,10000,1,1);';
  }
  else if (type == "card")
  {
    o += 'WindowOpen(\'' + url + '\',\'CARD\',\'location=yes,directorybuttons=no,scrollbars=yes,resizable=yes,menubar=yes,toolbar=yes\');';
  }
  else
  {
    o += 'ChangeContent(\'' + url + '\');';
  }
  o += '" >' + GL(key) + '</span>';
  return o;
};

function CreateSystemMenu(extMenu)
{
  DW(CommonGetMenuStr(MENU_ITEM_SYSTEM, extMenu));
};
function CreateStorageMenu(extMenu)
{
 DW(CommonGetMenuStr(MENU_ITEM_EVENT, extMenu));
}
function CreateImageMenu(extMenu)
{
  DW(CommonGetMenuStr(MENU_ITEM_IMAGE, extMenu));
};

function GetTagAX1AndFixSize(id)
{
  var o = '';
  // 20071024 Luther add, use common way
  if (g_viewXSize >= 640 && g_viewYSize < 300)
  {
    o += TagAX1(id, g_viewXSize, g_viewYSize * 2);
  }
  else
  {
	var w = g_viewXSize;
    var h = g_viewYSize;
    if (w > 720)
    {
      h = (720 * h) / w;
      w = 720;
    }
    o += TagAX1(id, w, h);
  }
  return o;
};
function GetHalfViewSizeX()
{
  return 352;
};
function GetHalfViewSizeY()
{
  return (352 * g_viewYSize) / g_viewXSize;
};

// return the fix length string, if the length is too short , it will add the insChar.
// type:,0 insert insChar to left, the string will align right,
// type: 1 align left
// type:2 middle
function GetFixLenStr(str, len, insChar, type)
{
  var o = "";
  str = "" + str; // make sure it is string
  var count = len - str.length;
  var lc = rc = 0;
  if (type == 1)
  {
    rc = count;
  }
  else if (type == 2)
  {
    lc = count / 2;
    rc = count - lc;
  }
  else
  {
    lc = count;
  }
  if (lc > 0)
  {
    var i = 0;
    for (i = 0; i < lc; i++)
    {
      o += insChar;
    }
  }
  o += str;
  if (rc > 0)
  {
    var i = 0;
    for (i = 0; i < rc; i++)
    {
      o += insChar;
    }
  }
  return o;
};

function DisableObject(name, isDisable)
{;
  var obj = GE(name);
  if (obj != null)
  {
    obj.disabled = isDisable;
  }
  try
  {
    SetCIA(name, !isDisable);
  }
  catch (e)
  {};

  var objs = GES(name);
  if (objs != null)
  {
    for ( var i = 0; i < objs.length; i++)
    {
      objs[i].disabled = isDisable;
    }
  }
};
function DisableObjs(list, isDisable)
{
  for ( var i = 0; i < list.length; i++)
  {
    DisableObject(list[i], isDisable);
  }
};

// get init submit string
// return the submit POSTDATA init data.
function GetIS()
{
  var o = 'GET /' + THIS_PAGE + '?';
  o += "language=ie";
  return o;
};

// set the object visible
function SetVisible(name, isVisible)
{
  var obj = GE(name);
  if (obj != null)
  {
    obj.style.visibility = (isVisible) ? keyword_Show : keyword_Hide;
  }
};

// change size to Bytes,MB,GB ex: 1035 = 1.02 KB
// fixDotSize decimal point postion , default is 2
function GetCapacityString(size, fixDotSize)
{
  var BASEUNIT = 1024;
  var capStr = '';
  if (fixDotSize == null || fixDotSize < 0)
  {
    fixDotSize = 2;
  }
  var total = parseFloat(size);
  if (total < BASEUNIT)
  {
    capStr = total + ' Bytes';
  }
  else
  {
    total /= BASEUNIT;
    total = total.toFixed(fixDotSize);
    if (total == parseFloat(total))
    {
      total = parseFloat(total);
    }
    if (total < BASEUNIT)
    {
      capStr = total + ' KB';
    }
    else
    {
      total /= BASEUNIT;
      total = total.toFixed(fixDotSize);
      if (total == parseFloat(total))
      {
        total = parseFloat(total);
      }
      if (total < BASEUNIT)
      {
        capStr = total + ' MB';
      }
      else
      {
        total /= BASEUNIT;
        total = total.toFixed(fixDotSize);
        if (total == parseFloat(total))
        {
          total = parseFloat(total);
        }
        capStr = total + ' GB';
      }
    }
  }
  return capStr;
};

// input data object and format id isDayLight ,return time string.
// timeFormat :
// 0-> YY-MM-DD
// 1-> MM-DD-YY
// 2-> DD-MM-YY
function GiveMeDateTimeString(dateObj, timeFormat, isDayLight)
{
  return ((GiveMeDateString(dateObj, timeFormat, isDayLight)) + " " + (GiveMeTimeString(dateObj)));
};
function GiveMeDateString(dateObj, timeFormat, isDayLight)
{
  var y, M, d;
  if (browser_FireFox)
  {
    y = FixNum(dateObj.getYear() + 1900, 4);
  }
  else
  {
    y = FixNum(dateObj.getYear(), 4);
  }
  M = FixNum(dateObj.getMonth() + 1, 2);
  d = FixNum(dateObj.getDate(), 2);

  var o = "";
  if (isDayLight == 1)
    o += "(+) ";

  if (timeFormat == 1)
  {
    o += M + "/" + d + "/" + y;
  }
  else if (timeFormat == 2)
  {
    o += d + "/" + M + "/" + y;
  }
  else
  {
    o += y + "/" + M + "/" + d;
  }
  return o;
};

function GiveMeTimeString(dateObj)
{
  var h, m, s;
  h = FixNum(dateObj.getHours(), 2);
  m = FixNum(dateObj.getMinutes(), 2);
  s = FixNum(dateObj.getSeconds(), 2);
  return (h + ":" + m + ":" + s);
};

function FixNum(str, len)
{
  return FixLen(str, len, "0");
};
function FixLen(str, len, c)
{
  var ins = "";
  var i;
  for (i = 0; i < len - str.toString().length; i++)
  {
    ins += c;
  }
  return ins + str;
};

function CommonGetMenuStr(items, extMenu)
{
  var o = '';
  o += '<table width="150" border="0" valign="top" >';
  var i;
  for (i = 0; i < items.length; i += 3)
  {
    o += CreateSystemMenuItem(items[i], items[i + 1], items[i + 2]);
  }
  if (extMenu != null)
  {
    for (i = 0; i < extMenu.length / 2; i++)
    {
      o += CreateSystemMenuItem(extMenu[i * 2], extMenu[(i * 2) + 1]);
    }
  }
  o += '</table>';
  return o;
};

function GetMenuNetworkStr(extMenu)
{
  return CommonGetMenuStr(MENU_ITEM_NETWORK, extMenu);
};

function GetMenuAppStr()
{
  //APP has three type: setting, record, alarm
  var o = '';
  o += '<table width="150" border="0" align="right" valign="top" >';
  o += GetMenuAppSubStr(MENU_ITEM_APP_SET, GL("setting"));
  o += GetMenuAppSubStr(MENU_ITEM_APP_REC, GL("record"));
  o += GetMenuAppSubStr(MENU_ITEM_APP_ALARM, GL("alarm"));
  o += '</table>';
  return o;
};
function GetMenuAppSubStr(ITEMS, subName)
{
  if (GetFirstOKLink(ITEMS) == null)
    return "";
  var o = '';
  o += '<tr class="m1"><td height="20" colspan="2">' + subName + '</td></tr>';
  var i;
  for (i = 0; i < ITEMS.length; i += 3)
  {
    o += CreateSystemMenuItem(ITEMS[i], ITEMS[i + 1], ITEMS[i + 2]);
  }
  return o;
};

function ChangeImageLeftVideo(useAX)
{
  GE("imgAX").innerHTML = GetImageLeftVideo(useAX);
};
function GetImageLeftVideo(useAX)
{
  var o = "";
  if (IsMpeg4() || useAX == 1 || useAX == true)
  {
    if (IsVS())
    {
      o = TagAX2(1, GetHalfViewSizeX(), GetHalfViewSizeY());
    }
    else
    {
      o = TagAX2(null, GetHalfViewSizeX(), GetHalfViewSizeY());
    }
  }
  else
  {
    o += '<table border=3 bordercolor="#99CCFF"><tr><td>';
    o += imgFetcher.GetDmsImgStr(GetHalfViewSizeX(), GetHalfViewSizeY());
    o += '</td></tr></table>';
  }
  return o;
};
// vType is user define variable 0 ,store the mjpeg mode , display control is ActiveX or JavaApplet
// isNoShowWaitImg , is in changing don't show the waitting img?
function WriteImageLeftSide(vType, isNoShowWaitImg, extMenu)
{
  //DW('<td  width=400 height=350 aligh=left>');
  //DW('<div style="width:200">');
  //CreateImageMenu(extMenu);
  DW('<center><div id="imgAX">');
  // alert("Vt:"+vType+",sw:"+isNoShowWaitImg);
  if (isNoShowWaitImg)
  {
     //DW(GetImageLeftVideo(vType));
     DW('<OBJECT classid="clsid:9BE31822-FDAD-461B-AD51-BE1D1C159921" codebase="http://downloads.videolan.org/pub/videolan/vlc/latest/win32/axvlc.cab#Version=0,8,6,0" width="320 height="240" id="vlc"></OBJECT>');
  }
  else
  {
    DW("<img id='sampleimg' name='sampleimg' src='/loading.gif' width=352 height=240 border=2>");
  }
  DW('</div></center></td>');
};

// get Select Object ,be select text not index
function GetSelectText(name)
{
  var oo = "";
  var obj = GE(name);
  if (obj != null)
  {
    var ix = obj.selectedIndex;
    if (ix >= 0)
    {
      oo = obj.options[obj.selectedIndex].text;
    }
  }
  return oo;
};

function CheckBadFqdnLen(str, msg)
{
  var result = false;
  if (str.length > v_maxFqdnLen)
  {
    alert(msg);
    result = true;
  }
  return result;
};

// ===========================================
// I18N,
// ===========================================
// GL : GetLanguage
// name: is the key of the i18n string,
function GL(name, va)
{
  name = LangKeyPrefix + name;
  var v = (typeof (LAry[name]) == "undefined") ? GetDefaultStr(name) : LAry[name], n;

  if (va)
  {
    for (n in va)
      v = this.ReplaceVar(v, n, va[n]);
  }
  return v;
};

// string replace
// the {$xxx} will be treat a variable, and replace it.
// h: the origional string, r:replace key(no $ sign.),v: replace value.
function ReplaceVar(h, r, v)
{
  //debug
  // alert("h="+h+"r="+r+"v="+v);
  // alert(h.replace(new RegExp('{\\\$' + r + '}', 'g'), v));
  return h.replace(new RegExp('{\\\$' + r + '}', 'g'), v);
};

function GetDefaultStr(v)
{
  v = v.substring(LangKeyPrefix.length, v.length);
  return v.replace(new RegExp("_", "g"), " ");
};

function AddLangs(ar)
{
  for ( var key in ar)
  {
    if (typeof (ar[key]) == 'function')
      continue;
    // debug
    // alert((key.indexOf(LangKeyPrefix) == -1 ? LangKeyPrefix : '') + key);
    // alert(ar[key]);
    LAry[(key.indexOf(LangKeyPrefix) == -1 ? LangKeyPrefix : '') + key] = ar[key];
  }
};

// replace content key word to I18N .
function I18NHtml(src)
{
  var len = LangKeyPrefix.length;
  for ( var key in LAry)
  {
    //alert(key.substring(len,key.lenght) +" : "+LAry[key]);
    src = ReplaceVar(src, key.substring(len, key.length), LAry[key]);
  }
  //alert(src);
  return src;
};
// ===========================================
// about UI setting
// ===========================================
// AddLightBtn : add a light button
// GetLight : get the light status. 0:off 1:on
// SwitchLight : switch the light status, on -> off , or off -> on
// SetLightPos : set the light position.

// add a graphic buttion, can display on-off,
// it will put the image in <div> tag.
// id : the light id
// onChangeFun is a function name, ex: turnOn(), if you want to add args,
// use \"\", trunOn(\"1000\")
function AddLightBtn(id, left, top, width, height, onImg, offImg, onChangeFunc, describe)
{
  var o = "";
  o += "<div id='LIGHT_" + id + "' style='position:absolute; left:" + left + "px; top:" + top + "px; width:" + width + "px; height:" + height + "px; z-index:10'><img id='LIGHT_PIC_" + id + "' src='" + offImg + "' onMouseDown='SwitchLight(\"" + id + "\");' style='cursor:pointer ' alt='" + describe + "' onMouseOver='WS(\"" + describe + "\");' onMouseLeave='WS();'></div>";
  DW(o);
  LightAry["LIGHT_" + id] = new LightInfo(id, onImg, offImg, onChangeFunc);
};

function SwitchLight(id, noAct)
{
  var e = GE("LIGHT_PIC_" + id);
  // alert(e);
  if (e != null)
  {
    var obj = LightAry["LIGHT_" + id];
    if (obj != null)
    {
      //alert(obj);
      e.src = obj.switchNext(noAct);
    }
  }
};
function GetLight(id)
{
  var e = LightAry["LIGHT_" + id];
  var res = 0;
  if (e != null)
  {
    res = e.value;
  }
  //alert(e.lowsrc);
  return res;
};
function SetLight(id, val, noAct)
{
  if (GetLight(id) != val)
  {
    //alert(GetLight(id));
    SwitchLight(id, noAct);
  }
};
function SetLightPos(id, x, y)
{
  var e = GE("LIGHT_" + id);
  if (e != null)
  {
    e.style.left = x;
    e.style.top = y;
  }
  //alert(e+":"+x+":"+y);
};
// the light information object
function LightInfo(id, onImg, offImg, chgFun)
{
  this.id = id;
  this.onImg = onImg;
  this.offImg = offImg;
  this.value = 0;
  this.onChangeFunc = chgFun;
  this.switchNext = LightSwitchNext;
};
// switch to next status, and return the new image.
function LightSwitchNext(noAct)
{
  var res;
  if (this.value == 0)
  {
    this.value = 1;
    res = this.onImg;
  }
  else
  {
    this.value = 0;
    res = this.offImg;
  }
  if (this.onChangeFunc != null && noAct != true)
  {
    if (!this.onChangeFunc())
    {
      //recover
      if (this.value == 0)
      {
        this.value = 1;
        res = this.onImg;
      }
      else
      {
        this.value = 0;
        res = this.offImg;
      }
    }

  }
  return res;
};
var LightAry = {};

// ===========================================
// about Machine type
// ===========================================
function IsVS()
{
  var res = false;
  // res = (g_machineCode == "1290" ||g_machineCode == "1280");
  res = (g_machineCode == "1290" || g_machineCode == "1291");

  // res = true;

  return res;
};
function Is2114()
{
  //return (g_machineCode == "1291");
  return (g_isSupportMQ && (g_quadMode == 0));
};

// ===========================================
// Dynamic get content. XMLHttpRequest...
// ===========================================
// use XMLHttpRequest to get the content, and put in center web page.
// isForceChange means do not check last page name, just reload it.
function ChangeContent(link, isNoHis, isForceChange)
{
  if (link != null)
  {
    link = link.toLowerCase();
  }
  //WS((debugC++)+" : "+link);
  if (IsInBadLinkList(link))
  {
    return;
  }
  
  if (g_lockLink)
  {
    WS("LOCKED...");
    return;
  }
  g_lockLink=true;
 
  if (link == null)
  {
    link = CONTENT_PAGE;
  }
  else
  {
    CONTENT_PAGE_LAST = CONTENT_PAGE;
    if (isForceChange != true && CONTENT_PAGE == link)
    {
      //alert("link:"+link+":CP:"+CONTENT_PAGE);
      // avoid endless loop
      g_lockLink = false;
      return;
    }

    if (isNoHis != true)
    {
      if (g_backList.length > 0)
      {
        if (g_backList[g_backList.length - 1] != link)
        {
          if (g_fwdList.length > 0)
          {
            //alert(g_fwdList[g_fwdList.length-1]+"(11pop):"+g_backList[g_backList.length-1]+"(push)");
            g_backList.push(g_fwdList.pop());
          }
          //alert("11:g_backList(push) :"+link);
          g_backList.push(link);
        }

      }
      else
      {
        if (g_fwdList.length > 0)
        {
          //alert(g_fwdList[g_fwdList.length-1]+"(22pop):"+g_backList[g_backList.length-1]+"(push)");
          g_backList.push(g_fwdList.pop());
        }

        //alert("22:g_backList(push) :"+link);
        g_backList.push(link);
      }
      //      if (g_fwdList.length > 0 )
      // {
      // g_backList.push(g_fwdList.pop());
      // }
      g_fwdList = new Array();
    }

  }
  //20061211 move StopActiveX() to last, if you repeat to click the "image"
  // it will let ActiveX to stop , and can not restart.
  try
  {
    StopActiveX();
  }
  catch (e)
  {};

  CONTENT_PAGE = link;
  if (WCH != null)
  {
    //WCH.onreadystatechange=null;
    delete WCH;
    WCH = null;
  }
  WCH = InitXHttp();
  WCH.onreadystatechange = OnWebContentProcess;
  try
  {
    //if((typeof(g_isStartPtz) != "undefined")&& (g_isShowDvPtzCtrl))
    // {
    // ShowPtzCtrl(false);
    // }
    WCH.open("GET", "/" + link, true);
    WCH.setRequestHeader("If-Modified-Since", "0");
    // finally send the call
    WCH.send(null);
    // alert("ChangeContent:"+data);
  }
  catch (e)
  {
    alert(GL("err_get_content"));
    CONTENT_PAGE = CONTENT_PAGE_LAST;
  }

  //g_lockLink = false;
};

function FoundBadLink(link)
{
  if (link == null || link == "")
    return true;
  var badStr = ":/.%$#@!~^&*(){}|\\;\"'?><,";
  for ( var i = 0; i < badStr.length; i++)
  {
    if (link.indexOf(badStr.charAt(i)) >= 0)
      return true;
  }
  return false;
};

// ALC , After Load Content
// Note: this function must call in the end of the each page.
function ALC()
{
  WS("");
  g_lockLink = true;
  setTimeout(MY_ONLOAD, 300);
};

function GetViewCHHtml()
{
  var o = '';
  if (IsVS())
  {
    o += '<div id="viewChannelLayer" class="cssViewChLayer">';
    o += GL('view') + ': ' + GetCHSelHtml();
    o += '</div>';

  }

  return o;
};

function OnWebContentProcess()
{
  if (WCH == null)
    return;
  if (WCH.readyState == 4)
  {
    if (WCH.status == 200 || WCH.status == 401 || WCH.status == 404 || WCH.status == 403)
    {
      //alert(WCH.responseText);
      if (WCH.responseText.indexOf("var.js") >= 0)
        return;
      // var body = GE("WebContent");
      // var o = '<span style="display: none">'+I18NHtml(WCH.responseText)+'</span>';
      var o = '';
      // o+=GetViewCHHtml()+GetEmptyCallback()+I18NHtml(WCH.responseText);
      o += GetEmptyCallback() + I18NHtml(WCH.responseText);
      // alert(o);

      // call unload function.
      CallOnUnload();
      // set_innerHTML('WebContent',o);
      setInnerHTML(GE('WebContent'), o);
      setTimeout(CallOnResize, 500);
      // GE("WebContent").innerHTML='<span style="display: none">'+I18NHtml(WCH.responseText)+'</span>';
      if (WCH.status != 200)
      {
        g_lockLink = false;
      }
    }
    else
    {
      alert(GL("err_get_content"));
      CONTENT_PAGE = CONTENT_PAGE_LAST;
      g_lockLink = false;
    }

  }
};

function GetEmptyCallback()
{
  var o = '';
  o += '<scri' + 'pt>function MY_ONUNLOAD(){};function MY_ONLOAD(){g_lockLink = false;};function MY_CH_CHANGE(){};';
  o += 'function MY_SUBMIT_OK(){};function MY_ONRESIZE(){};function MY_BEFORE_SUBMIT(){};</scri' + 'pt>';
  return o;
};

// ===========================================
// call to content script.
// ===========================================
function CallOnUnload()
{
  //alert("CallOnUnload");
  g_dmsRun = false;
  clearTimeout(jpegTimer);
  jpegTimer = null;

  if (typeof (MY_ONUNLOAD) == 'function')
    MY_ONUNLOAD();
};

function CallOnResize()
{

  var obj = null;
  GetBaseXY();
  obj = GE("logoLayer");
  if (obj != null)
  {
    //var tmpX = halfWidth - 370;
    var tmpX = baseX + 10;
    if (g_brandName == "ipx")
    {
      tmpX += 30;
    };
    obj.style.left = tmpX;
  }
  obj = GE("arrowImg");
  if (obj != null)
  {
    //obj.style.left = halfWidth + 270;
	obj.style.left = baseX + 660;
    obj.style.top = 520;
    var l, r;
    if (CONTENT_PAGE == "main.htm" || CONTENT_PAGE == "c_help.htm")
    {
      g_backList = new Array();
      g_fwdList = new Array();
      obj.style.left = -600;
    }
    else
    {
      var b = (g_backList.length > 0);
      if (g_backList.length == 1 && g_backList[0] == CONTENT_PAGE)
        b = false;
      var f = (g_fwdList.length > 0);
      if (b && f)
      {
        l = 0;
        r = 90;
      }
      else if (b)
      {
        l = 0;
        r = 60;
      }
      else if (f)
      {
        l = 30;
        r = 90;
      }
      else
      {
        l = 30;
        r = 60;
      }
      obj.style.clip = "rect(0 " + r + " 30 " + l + ")";
    }

  }
  if (IsVS())
  {
    obj = null;
    obj = GE("viewChannelLayer");
    if (obj != null)
    {
      obj.style.left = halfWidth + 250;
    }
    SetVisible("viewChannelLayer", (g_CHPageList.indexOf(CONTENT_PAGE + ";") >= 0));
  }
  if (g_isShowPtzCtrl)
  {
    FixPtzButtonPos((CONTENT_PAGE == "main.htm"));
  }
  if (typeof (MY_ONRESIZE) == 'function')
    MY_ONRESIZE();
  // g_lockLink=false;
};

// ===========================================
// Wrap the control to object.
// ===========================================
// SetCCV : SetCommonContrlValue
function SetCCV(id, val)
{
  SetCtrlValue(CTRLARY, id, val);
};
function SetCtrlValue(ctrlList, id, val)
{
  var obj = ctrlList[id];
  if (obj != null)
  {
    obj.SV(val);
  }
};
// GetCCV : GetCommonContrlValue
function GetCCV(id)
{
  return GetCtrlValue(CTRLARY, id);
};
function GetCtrlValue(ctrlList, id)
{
  var res = '';
  var obj = ctrlList[id];
  if (obj != null)
  {
    res = obj.GV();
  }
  return res;
};
// Set Control InActive
function SetCIA(id, val)
{
  CTRLARY[id].active = (val == true);
};
function GetCtrlHtml(ctrlList, id)
{
  var res = '';
  var obj = ctrlList[id];
  if (obj != null)
  {
    if (obj.type == 'radio')
    {
      res = GetRadioONOFF(ctrlList, id);
    }
    else
    {
      res = obj.html;
    }
  }
  return res;
};

function GetHtmlbyId(id)
{
  var res = '';
  var obj = CTRLARY[id];
  if (obj != null) {
    res = obj.GetHtml();
  } else {
    res = "Error: GetHtmlbyId";
  }
  return res;
}

function WH(id)
{
  DW(WH_(id));
};
function WH_(id)
{
  return GetCtrlHtml(CTRLARY, id);
};
function GetRadioCtrlHtml(ctrlList, id, val)
{
  var res = '';
  var obj = ctrlList[id];
  if (obj != null)
  {
    res = obj.GetHtml(val);
  }
  return res;
};

function WRH(id, val)
{
  DW(WRH_(id, val));
};
function WRH_(id, val)
{
  return GetRadioCtrlHtml(CTRLARY, id, val);
};
function CPASS(ctrl, isQuite)
{
  var res = true;
  if (ctrl.checker != null)
  {
    res = ctrl.checker.IsPass(ctrl, isQuite);
  }
  return res;
};
function Ctrl_Select(id, list, val, setcmd, onChangeFunc, checker, inactive)
{
  this.type = "select";
  this.active = !(inactive == true);
  this.id = id;
  this.list = list;
  this.value = val;
  this.setcmd = setcmd;
  this.checker = checker;
  this.html = SelectObjectNoWrite(id, list, val, onChangeFunc);
  // GetValue
  this.GV = function()
  {
    return GetValue(this.id);
  };
  // SetValue
  this.SV = function(val)
  {
    SetValue(this.id, val);
  };
  this.IsPass = function(isQuite)
  {
    return CPASS(this, isQuite);
  };
};
function Ctrl_SelectNum(id, sNum, eNum, gap, val, setcmd, onChangeFunc, onFocusFunc, fixNumC, checker, inactive)
{
  this.type = "selectNum";
  this.active = !(inactive == true);
  this.id = id;
  this.value = val;
  this.setcmd = setcmd;
  this.checker = checker;
  this.html = GetSelectNumberHtml(id, sNum, eNum, gap, val, onChangeFunc, onFocusFunc, fixNumC);
  // GetValue
  this.GV = function()
  {
    return GetValue(this.id);
  };
  // SetValue
  this.SV = function(val)
  {
    SetValue(this.id, val);
  };
  this.IsPass = function(isQuite)
  {
    return CPASS(this, isQuite);
  };

};
function Ctrl_Text(id, size, maxlen, val, setcmd, checker, isPwd, onChangeFunc, inactive, onKeyUP)
{
  this.type = "text";
  this.active = !(inactive == true);
  this.id = id;
  this.value = val;
  this.setcmd = setcmd;
  this.checker = checker;
  this.html = CreateTextHtml(id, size, maxlen, val, isPwd, onChangeFunc, onKeyUP);
  // GetValue
  this.GV = function()
  {
    return GetValue(this.id);
  };
  // SetValue
  this.SV = function(val)
  {
    SetValue(this.id, val);
  };
  this.IsPass = function(isQuite)
  {
    return CPASS(this, isQuite);
  };

};
function Ctrl_Check(id, val, setcmd, onClickFunc, checker, inactive)
{
  this.type = "check";
  this.active = !(inactive == true);
  this.id = id;
  this.value = val;
  this.setcmd = setcmd;
  this.checker = checker;
  this.onClickFunc = onClickFunc;
  this.GetHtml = function()
  {
    var o = '';
    o += '<input type="checkbox" id="' + this.id + '" name="' + this.id + '" ';
    if (this.value == 1)
    {
      o += "checked ";
    }
    if (this.active != true)
    {
      o += "disabled='true'";
    }
    if (this.onClickFunc != null)
    {
      o += "onClick='" + this.onClickFunc + "'";
    }
    o += " >";
    return o;
  };
  this.html = this.GetHtml();
  // GetValue
  this.GV = function()
  {
    return Bool2Int(IsChecked(this.id));
  };
  // SetValue
  this.SV = function(val)
  {
    SetChecked(this.id, (val == 1));
  };
  this.IsPass = function(isQuite)
  {
    return CPASS(this, isQuite);
  };

};
function Ctrl_Radio(id, val, setcmd, onClickFunc, checker, inactive)
{
  this.type = "radio";
  this.active = !(inactive == true);
  this.id = id;
  this.value = val;
  this.setcmd = setcmd;
  this.checker = checker;
  this.onClickFunc = onClickFunc;
  this.GetHtml = function(val)
  {
    var o = "<input type='radio' name='" + this.id + "' id='" + this.id + "' class='m1' value='" + val + "' ";
    if (this.value == val)
    {
      o += "checked ";
    }
    if (this.onClickFunc != null)
    {
      o += "onClick='" + this.onClickFunc + "'";
    }
    o += " >";
    return o;
  };
  // GetValue
  this.GV = function()
  {
    return GetRadioValue(this.id);
  };
  // SetValue
  this.SV = function(val)
  {
    SetRadioValue(this.id, val);
  };
  this.IsPass = function(isQuite)
  {
    return CPASS(this, isQuite);
  };

};
function GetRadioONOFF(id)
{
  return GetRadioONOFF(CTRLARY, id);
};
function GetRadioONOFF(list, id)
{
  var o = '';
  o += GetRadioCtrlHtml(list, id, 1);
  o += GL("on");
  o += GetRadioCtrlHtml(list, id, 0);
  o += GL("off");
  return o;
};
// enable or disable
function GetRadioENDIS(id)
{
  var o = '';
  o += WRH_(id, 1);
  o += GL("enable");
  o += WRH_(id, 0);
  o += GL("disable");
  return o;
};
// AD: Allow/Deny
function GetRadioAD(id)
{
  var o = '';
  o += WRH_(id, 1);
  o += GL("allow");
  o += WRH_(id, 0);
  o += GL("deny");
  return o;
};
// note: onChangeFunc not implement
function Ctrl_IPAddr(id, val, setcmd, onChangeFunc, checker, inactive)
{
  this.type = "ipaddr";
  this.active = !(inactive == true);
  this.id = id;
  this.value = val;
  this.setcmd = setcmd;
  this.checker = checker;
  this.onChangeFunc = onChangeFunc;
  this.GetIP = function(s)
  {
    if (s.charAt(0) == '0')
    {
      s = s.replace('0', '');
      if (s.charAt(0) == '0')
      {
        s = s.replace('0', '');
      }
    }
    if (s == "")
      return 0;
    return parseInt(s);
  };
  this.GV = function()
  {
    var o = '';
    for ( var i = 1; i < 5; i++)
    {
      try
      {
        o += FixNum(this.GetIP(GE(this.id + '_ip' + i).value), 3);
        o += (i < 4) ? "." : "";
      }
      catch (e)
      {};
    }
    this.value = o;
    // alert(o.replace(/\./g,""));
    return o.replace(/\./g, "");
  };
  this.SV = function(val)
  {
    var ips = val.split(".");
    if (ips.length >= 4)
    {
      for ( var i = 1; i < 5; i++)
      {
        try
        {
          GE(this.id + '_ip' + i).value = this.GetIP(ips[i - 1]);
        }
        catch (e)
        {};
      }
      this.value = val;
    }
  };
  this.GetHtml = function()
  {
    var ips = this.value.split(".");
    // alert(ips.length);
    var o = '';
    o += '<div class="cssIpAddr" id="' + this.id + '">';
    for ( var i = 1; i < 5; i++)
    {
      //alert(ips[i-1]);
      o += '<input type="text" value="' + ((ips.length >= 4) ? this
          .GetIP(ips[i - 1]) : '0') + '" name="' + this.id + '_ip' + i + '" id="' + this.id + '_ip' + i + '" maxlength=3 class="cssIpAddrItem" onkeyup="IPMask(this,event)" onkeydown="IPMaskDown(this,event)" onblur="if(this.value==\'\')this.value=\'0\';" onbeforepaste=IPMask_c()>';
      o += (i < 4) ? "." : "";
    }
    o += '</div>';
    return o;
  };
  this.html = this.GetHtml();
  this.IsPass = function(isQuite)
  {
    return CPASS(this, isQuite);
  };
  this.Disable = function(isDis)
  {
    this.active = !isDis;
    for ( var i = 1; i < 5; i++)
      DisableObject(this.id + '_ip' + i, isDis);
  }

};
// note: onChangeFunc not implement
function Ctrl_IPFilter(id, val, setcmd, onChangeFunc, checker, inactive)
{
  this.type = "ipfilter";
  this.active = !(inactive == true);
  this.id = id;
  this.value = val;
  this.setcmd = setcmd;
  this.checker = checker;
  this.onChangeFunc = onChangeFunc;
  this.GetIP = function(s)
  {
    if (s.charAt(0) == '0')
    {
      s = s.replace('0', '');
      if (s.charAt(0) == '0')
      {
        s = s.replace('0', '');
      }
    }
    if (s == "")
      return 0;
    // return parseInt(s);
    return s;
  };
  this.GV = function()
  {
    var o = '';
    for ( var i = 1; i < 5; i++)
    {
      try
      {
        o += this.GetIP(GE(this.id + '_ip' + i).value);
        o += (i < 4) ? "." : "";
      }
      catch (e)
      {};
    }
    this.value = o;
    return o;
  };
  this.SV = function(val)
  {
    var ips = val.split(".");
    if (ips.length >= 4)
    {
      for ( var i = 1; i < 5; i++)
      {
        try
        {
          GE(this.id + '_ip' + i).value = this.GetIP(ips[i - 1]);
        }
        catch (e)
        {};
      }
      this.value = val;
    }
  };
  this.GetHtml = function()
  {
    var ips = this.value.split(".");
    // alert(ips.length);
    var o = '';
    o += '<div class="cssIpFilter" id="' + this.id + '">';
    for ( var i = 1; i < 5; i++)
    {
      //alert(ips[i-1]);
      o += '<input type="text" value="' + ((ips.length >= 4) ? this
          .GetIP(ips[i - 1]) : '0') + '" name="' + this.id + '_ip' + i + '" id="' + this.id + '_ip' + i + '" maxlength=9 class="cssIpFilterItem" onkeyup="IPMask(this,event,true)" onkeydown="IPMaskDown(this,event)" onblur="if(this.value==\'\')this.value=\'0\';" onbeforepaste=IPMask_c(true)>';
      o += (i < 4) ? "." : "";
    }
    o += '</div>';
    return o;
  };
  this.html = this.GetHtml();
  this.IsPass = function(isQuite)
  {
    return CPASS(this, isQuite);
  };
  this.Disable = function(isDis)
  {
    for ( var i = 1; i < 5; i++)
      DisableObject(this.id + '_ip' + i, isDis);
  };

};

// ===========================================
// IP Mask control
// ===========================================
function FocusToNextIPAddr(obj, isNext, select)
{
  //alert(obj+":"+isNext);
  obj.blur();
  var nextip = parseInt(obj.name.charAt(obj.name.length - 1));
  nextip = (isNext) ? nextip + 1 : nextip - 1;
  nextip = nextip >= 5 ? 1 : nextip;
  nextip = nextip <= 0 ? 4 : nextip;
  try
  {
    var oo = GE(obj.name.substring(0, obj.name.length - 1) + nextip);
    oo.focus();
    if (select)
    {
      oo.select();
    }
  }
  catch (e)
  {};

};
function IPMaskDown(obj, evt)
{
  var key1 = evt.keyCode;
  if (key1 == 190 || key1 == 110)
  {
    FocusToNextIPAddr(obj, true, false);
  }
};
function IPMask(obj, evt, isFilter)
{
  var key1 = evt.keyCode;
  // alert(key1);
  if (!(key1 == 46 || key1 == 8 || key1 == 48 || (key1 >= 49 && key1 <= 57) || (key1 >= 97 && key1 <= 104)))
  {
    if (isFilter == true)
    {
      obj.value = obj.value.replace(/[^\d\-\*\(\)]/g, '');
    }
    else
    {
      obj.value = obj.value.replace(/[^\d]/g, '');
    }
  }

  if (key1 == 190 || key1 == 110)
  {
    obj.select();
  }
  //alert(key1);
  if (key1 == 37 || key1 == 39)
  {
    //alert("a:"+key1);
    FocusToNextIPAddr(obj, (key1 == 39), false);
  }
  if (obj.value.length >= 3)
  {
    if (parseInt(obj.value) >= 256 || parseInt(obj.value) < 0)
    {
      alert(GL("err_ip_address"));
      obj.value = "";
      obj.focus();
      return false;
    }
    //    else
    // {
    // alert("b:"+key1+":"+obj.value);
    // FocusToNextIPAddr(obj,true,true);
    // }
  }
};
function IPMask_c(isFilter)
{
  var txt = clipboardData.getData('text');
  txt = (isFilter == true) ? txt.replace(/[^\d\-\*\(\)]/g, '') : txt
      .replace(/[^\d]/g, '');
  clipboardData.setData('text', txt);
};
// ===========================================
// validate control and submit
// ===========================================
function GetSetterCmd(ctrl, val)
{
  return GetSetterCmdKV(ctrl.setcmd, val);
};
// direct input the name and value
function GetSetterCmdKV(name, val)
{
  var o = "&" + name;
  // Luther memo: it is important, null string is equals 0, :(
  if (isNaN(parseInt(val)) && val == "")
  {
    val = "(null)";
  }
  //if setcmd contain "=" ,it's mean video server do not add channel id.
  if (name.indexOf("=") < 0)
  {
    o += "=";
    if (IsVS())
    {
      o += g_CHID + ":";
    }
  }
  //add value
  o += val;
  return o;
};
function ValidateCtrlAndSubmit(ctrlList, isAsync)
{
  if (g_lockLink)
  {
    WS("LOCKED...");
    return;
  }
  if (isAsync == null)
    isAsync = false;
  // validate
  var res = true;
  for ( var key in ctrlList)
  {
    var obj = ctrlList[key];
    if (obj.active)
    {
      //      if (obj.checker != null)
      // {
      // res = obj.checker.IsPass(key,CTRLARY);
      res = obj.IsPass();
      if (!res)
      {
        return res;
      }
      //      }
    }
  }
  //call back
  if (MY_BEFORE_SUBMIT() == false)
  {
    return false;
  }

  //submit ,
  g_httpOK = true;
  var o = c_iniUrl;
  var len = g_MaxSubmitLen;
  for ( var key in ctrlList)
  {
    var obj = ctrlList[key];
    if (obj.active)
    {
      var str = GetSetterCmd(obj, GetCtrlValue(ctrlList, key));

      len -= str.length;
      o += str;
      if (len < 10)
      {
        //send submit
        // alert(o);
        SendHttp(o, isAsync);
        if (g_httpOK)
        {
          o = c_iniUrl;
          len = g_MaxSubmitLen;
        }
        else
        {
          break;
        }
      }

    }
  }
  if (len != g_MaxSubmitLen)
  {
    //alert(o);
    SendHttp(o, isAsync);
  }
  if (g_httpOK)
  {
    MY_SUBMIT_OK();
  }

};
// ===========================================
// Dispaly Message on browser status bar
// ===========================================
function WS(msg)
{
  window.status = (msg == null) ? "" : msg;
};
// ===========================================
// about video server channel
// ===========================================
// Get the channel id from the global variable "g_CHID".
function GetCHSelHtml()
{
  var o = "";

  o += "<select onChange='ChangeViewCH(this.value)' id='viewCHSel' class='m1'>";
  for ( var i = 0; i < 5; i++)
  {
    o += "<option value='" + i + "' " + ((i == g_CHID) ? "selected" : "") + ">" + ((i == 0) ? "---" : i) + "</option>";
  }
  o += "</select>";
  return o;
};
function ChangeViewCH(id)
{
  //focus lost
  window.focus();
  GE("viewCHSel").blur();

  if (g_CHID == 0 || id == 0)
  {
    if (CONTENT_PAGE == "main.htm")
    {
      g_CHID = id;
      ChangeContent();
    }
    else
    {
      // no change g_CHID
      SetValue("viewCHSel", g_CHID);
      MY_CH_CHANGE();
      // 20070419 Luther add 1 line
      ChangeChannelTextPanel(g_CHID);
    }
  }
  else
  {
    g_CHID = id;
    MY_CH_CHANGE();
    // 20070419 Luther add 1 line
    ChangeChannelTextPanel(g_CHID);
  }
};
function GetLogoHtml()
{
  var o = '';
  if (g_brandName != "nobrand")
  {
    o += '<div id="logoLayer" class="cssLogoLayer" >';
    var hasUrl = !(g_brandUrl == null || g_brandUrl == "" || g_brandUrl == "(null)");
    if (hasUrl)
    {
      o += '<a href="' + g_brandUrl + '" target="_blank">';
    }
    o += '<img border=0 src="' + g_brandName + '.gif">';
    if (hasUrl)
    {
      o += '</a>';
    }
    o += '</div>';
  }
  return o;
};
// ===========================================
// Common comtent
// ===========================================
// PH : Page Header
function GetPHHtml(key)
{
  return GetPHTxtHtml(GL(key));
};
// show text, not keyword.
function GetPHTxtHtml(txt)
{
  return '<br><center><span class=a1>' + txt + '</span></center>';
};
function WritePH(key)
{
  DW(GetPHHtml(key));
};
function WriteTxtPH(txt)
{
  DW(GetPHTxtHtml(txt));
};
// ===========================================
// Common Image Content
// ===========================================
// useAX: use ActiveX control ? yes or not,
// isOK: if in changing mode. isOK is false.
// PH : Page Header
function WriteImageTxtPH(txtTitle, useAX, isOK, extMenu)
{
  //WriteTxtPH(txtTitle);
  DW('<table width=480 align = "center" bordercolor="#FF9900"   border=1 ><tr>');
 // WriteImageLeftSide(useAX, isOK, extMenu); // auto complete <td></td>插件
  DW('<td vAlign=bottom width=252>');
};
// PB : Page Bottom
function WriteImagePB()
{
  DW('</td></tr></table>');
};

function VS_NO_VIEW_ALL()
{
  if (g_CHID == 0)
    g_CHID = 1;
  if (IsVS())
  {
    try
    {
      GE('viewCHSel').selectedIndex = g_CHID;
    }
    catch (e)
    {};
  }
};
function ImageOnLoad(isNoShowWaitImg, useAX, codec, mode, forceWait)
{
  VS_NO_VIEW_ALL();
  // CallOnResize();
  // alert(mode);
  var MP=0;
  if (isNoShowWaitImg)
  {
    if (useAX == 1 || IsMpeg4())
    {
      if (!IsVS())
      {
        if(g_isSupS3||g_isSupS6)
			MP = 3;
		else MP=1;
		StartActiveXEx(0, 0, codec, MP, null, mode, forceWait);
        // video server will start in MY_CH_CHANGE
      }
    }
    else
    {
      imgFetcher.RunDms();
    }
  }
  if (IsVS())
  {
    MY_CH_CHANGE();
  }

};
// ===========================================
// Common Netowrk Content
// ===========================================
// PH : Page Header
//////////////////////////////////////////////////////////////
function GetMenuEventworkStr(extMenu)
{
 return CommonGetMenuStr(MENU_ITEM_EVENT, extMenu);
}
function WriteEventPH(titleKey,extMenu)
{
WritePH(titleKey);
DW('<table width=680  border=0><tr><td width=180 valign="top">');
DW(GetMenuEventworkStr(extMenu));
DW('</td><td valign=top >');
}
///////////////////////////////////////////////////
function WriteNetPH(titleKey, extMenu)
{
  //WritePH(titleKey);
  DW('<table width=680 border=2><tr><td width=180 valign="top">');
  //DW(GetMenuNetworkStr(extMenu));
  DW('</td><td valign=top >');
};
// PB : Page Bottom
function WriteNetPB()
{
  DW('</td></tr></table>');
};

function WIPX(tid, ctx)
{
  DW('<tr class="b1"><td width=120 height="25" border="0">');
  DW(GL(tid) + ':</td><td border="0">' + ctx + '</td></tr>');
};

function WIP(tid, id)
{
  WIPX(tid, WH_(id));
};
function WIP1(ctx, css)
{
  DW('<tr class="' + ((css == null) ? "b1" : css) + '" width=150><td colspan=2 height="30" >');
  DW(ctx + '</td></tr>');
};
function WIPSubmit(isAsync)
{
  DW("<tr><td colspan=2 align=right>");
  CreateSubmitButton(null, isAsync);
  DW("</td></tr>");
};
// ===========================================
// Common Application Content
// ===========================================
// PH : Page Header
function WriteAppPH(titleKey)
{
  return WriteAppTxtPH(GL(titleKey))
};
function WriteAppTxtPH(txtTitle)
{
  WriteTxtPH(txtTitle);
  DW('<table width=680 border=0><tr><td width=180 valign="top">');
  DW(GetMenuAppStr());
  DW('</td><td valign=top >');
};

// PB : Page Bottom
function WriteAppPB()
{
  DW('</td></tr></table>');
};
function WTablePH(w)
{
  DW(TablePH_(w));
};
function TablePH_(w)
{
  return "<center><table width=" + ((w != null) ? w : "450") + " border=0 cellPadding=0 cellSpacing=0>";
}
function WTablePB()
{
  DW("</table></center>");
};
function WIApp(id, enid, ctx)
{
  DW("<tr><td height=30 class='b1'>");
  DW(WH_(id) + " " + GL(enid) + " - " + ctx + "");
  DW("</td></tr>");
};
function WIApp1(ctx)
{
  DW("<tr><td height=30 class='b1' style='text-indent:3em'>" + ctx + "</td></tr>");
};
function WIAppSubmit()
{
  DW("<tr><td ><center><br>");
  CreateSubmitButton();
  DW("</td></tr>");
};
// ===========================================
// Common System Content
// ===========================================
// PH : Page Header
function WriteSysPH(titleKey, addMenu)
{
  return WriteSysTxtPH(GL(titleKey), addMenu);
};
function WriteSysTxtPH(txtTitle, addMenu)
{
 // WriteTxtPH(txtTitle);
  DW('<table width=680 border=0><tr><td width=180 valign="top">');
 // CreateSystemMenu(addMenu);
  DW('</td><td valign=top >');
};
function WriteEvnTxtPH(txtTitle, addMenu)
{
 // WriteTxtPH(txtTitle);
  DW('<table width=680 border=0><tr><td width=180 valign="top">');
 // CreateStorageMenu(addMenu);
  DW('</td><td valign=top >');
};
// PB : Page Bottom
function WriteSysPB()
{
  DW('</td></tr></table>');
};

// ===========================================
// Checker Object
// ===========================================
// each checker has one validateObj and can link to another checker
function CheckerObj(validateObj, linkChecker)
{
  this.validateObj = validateObj;
  this.linkChecker = linkChecker;
  // this.ctrlList = ctrlList;
  this.IsPass = function(ctrlObj, isQuite)
  {
    var res = true;
    if (this.linkChecker != null)
    {
      res = this.linkChecker.IsPass(ctrlObj, isQuite);
    }
    if (res == true)
    {
      res = this.validateObj.IsPass(ctrlObj.GV(), isQuite);
    }
    if (res == false)
    {
      try
      {
        var obj = GE(ctrlObj.id);
        obj.focus();
        obj.select();
      }
      catch (e)
      {};
    }
    return res;
  };
};
// ValidateObj must implement IsPass(isQuite) return ture or false
// check the string length ,range (minLen <= x <= maxLen)
function V_StrLen(minLen, maxLen, defMsg, msg)
{
  //this.ctrlID = ctrlID;
  this.defMsg = defMsg;
  this.msg = msg;
  this.minLen = minLen;
  this.maxLen = maxLen;
  this.IsPass = function(val, isQuite)
  {
    return !(CheckBadStrLen(val, this.minLen, this.maxLen, this.defMsg, this.msg, isQuite));
  };
};
// check the string is English or number
function V_StrEnglishAndNumber(defMsg, msg, noCheckNum, noCheckLower, noCheckUpper)
{
  //this.ctrlID = ctrlID;
  this.defMsg = defMsg;
  this.msg = msg;
  this.noNum = noCheckNum;
  this.noLower = noCheckLower;
  this.noUpper = noCheckUpper;
  this.IsPass = function(val, isQuite)
  {
    return !(CheckBadEnglishAndNumber(val, this.defMsg, this.msg, this.noNum, this.noLower, this.noUpper, isQuite));
  };
};
// check the string is English or number
function V_StrFTPPath(defMsg, msg, noCheckNum, noCheckLower, noCheckUpper)
{
  //this.ctrlID = ctrlID;
  this.defMsg = defMsg;
  this.msg = msg;
  this.noNum = noCheckNum;
  this.noLower = noCheckLower;
  this.noUpper = noCheckUpper;
  this.IsPass = function(val, isQuite)
  {
    return !(CheckFTPPath(val, this.defMsg, this.msg, this.noNum, this.noLower, this.noUpper, isQuite));
  };
};
function V_NumRange(min, max, defMsg, msg)
{
  this.min = min;
  this.max = max;
  this.defMsg = defMsg;
  this.msg = msg;
  this.IsPass = function(val, isQuite)
  {
    return !(CheckBadNumberRange(val, this.min, this.max, this.defMsg, this.msg, isQuite));
  }
};
function V_Empty(defMsg, msg)
{
  this.defMsg = defMsg;
  this.msg = msg;
  this.IsPass = function(val, isQuite)
  {
    return !(CheckIsNull(val, this.defMsg, this.msg, isQuite));
  }
};
function V_EMail(msg)
{
  this.msg = msg;
  this.IsPass = function(val, isQuite)
  {
    return !(CheckBadEMail(val, this.msg, isQuite));
  }
};
function V_FQDN(msg)
{
  this.msg = msg;
  this.IsPass = function(val, isQuite)
  {
    return !(CheckBadFQDN(val, this.msg, isQuite));
  }
};

function V_Hex()
{
  this.IsPass = function(val, isQuite)
  {
    return (CheckHex(val, isQuite));
  }
};
// ===========================================
// Global validate object
// ===========================================
var gv_ipPort = new V_NumRange(1, 65535, "Port number");
var gco_ipPort = new CheckerObj(gv_ipPort);
var gv_byte = new V_NumRange(0, 255, "");
var gco_byte = new CheckerObj(gv_byte);
var gv_empty = new V_Empty(GL("text_field"));
var gco_empty = new CheckerObj(gv_empty);
var gv_email = new V_EMail();
var gco_email = new CheckerObj(gv_email, gco_empty);
var gv_FQDN = new V_FQDN();
var gco_FQDN = new CheckerObj(gv_FQDN, gco_empty);
var gv_engNum = new V_StrEnglishAndNumber("");
var gco_engNum = new CheckerObj(gv_engNum, gco_empty);
var gv_namePass = new V_StrEnglishAndNumber("Username/Password");
var gco_namePass = new CheckerObj(gv_namePass, gco_empty);
var gv_hex = new V_Hex();
var gco_hex = new CheckerObj(gv_hex, gco_empty);

function Hex2Dec(hex)
{
  return parseInt(hex, 16);
};
function Dec2Hex(dec)
{
  return parseInt(dec).toString(16).toUpperCase();
};
// ===========================================
// CSS style position.
// ===========================================
function SetPos(id, x, y, w, h)
{
  var obj = GE(id);
  if (obj != null && obj.style != null)
  {
    try
    {
      obj.style.width = w;
      obj.style.height = h;
      obj.style.top = y;
      obj.style.left = x;
    }
    catch (e)
    {};
  }
};
// ===========================================
// Index Page
// ===========================================
// Luther add for CMS access
function IndexCMSContent()
{
  DW('<link href="lc_en_us.csl" rel="stylesheet" type="text/css"></head><body><table><tr><td valign="top" width="745"><div id="WebContent" ></div></td></tr></table></body>');
  if (THIS_PAGE != null && THIS_PAGE.indexOf("?") >= 0)
  {
    //cms.htm?net&ch=1
    var ss = THIS_PAGE.split("?");
    // add channel parser
    if (ss.length > 1)
    {
      var s2 = ss[1].split("&");
      if (s2.length > 1)
      {
        var s3 = s2[1].split("=");
        if (s3[0] == "ch")
        {
          g_CHID = parseInt(s3[1]);
        }
      }
      if (ss[0] == s2[0] + ".htm")
      {
        ChangeContent(page);
      }
      else
      {
        ChangeContent(s2[0] + ".htm");
      }

    }

  }

};

function IndexContent(page, noHead)
{
  if (noHead != true)
  {
    WriteHtmlHead(null, null, null, "CallOnUnload", "CallOnResize");
  }
  WriteBottom();

  if (THIS_PAGE != null && THIS_PAGE.indexOf("?") >= 0)
  {
    var ss = THIS_PAGE.split("?");
    if (FoundBadLink(ss[1]))
    {
      ChangeContent(page);
    }
    else
    {
      //alert(ss[1]+":"+CONTENT_PAGE+":"+ss[0]);
      if (ss[0] == ss[1] + ".htm")
        ChangeContent(page);
      else
        ChangeContent(ss[1] + ".htm");
    }
  }
  else
  {
    ChangeContent(page);
  }

};

function MoveOnArrow(evt)
{
  var obj = GE("arrowImg");
  if (obj != null)
  {
    var v = parseInt(evt.x) - parseInt(obj.style.left);
    if (v < 30)
    {
      WS(GL("back"));
    }
    else if (v < 60)
    {
      WS(GL("reload"));
    }
    else
    {
      WS(GL("forward"));
    }
  }
};
function ClickArrow(evt)
{
  var obj = GE("arrowImg");
  if (obj != null)
  {
    if (g_lockLink)
    {
      WS("LOCKED...");
      return;
    }
    var v = parseInt(evt.x) - parseInt(obj.style.left);
    if (v < 30)
    {
      if (g_backList.length > 0)
      {
        var k = g_backList.pop();
        g_fwdList.push(k);
        while (k == CONTENT_PAGE)
        {
          k = g_backList.pop();
          g_fwdList.push(k);
        }
        if (k != null)
        {
          ChangeContent(k, true);
        }
      }
    }
    else if (v < 60)
    {
      ChangeContent(null, true);
    }
    else
    {
      if (g_fwdList.length > 0)
      {
        var k = g_fwdList.pop();
        g_backList.push(k);
        while (k == CONTENT_PAGE)
        {
          k = g_fwdList.pop();
          g_backList.push(k);
        }
        if (k != null)
        {
          ChangeContent(k, true);
        }
      }
    }

  }

};

// ============About JPEG Viewer===========
// use AJAX to replace Java Applet.
var g_maxDmsObj = 16;
var g_fetchList = {};
var jpegCounter = 0;
var jpegTimer = null;
var g_dmsRun = false;
var zeroFpsCount = 0;

function ImageFetcher(myid)
{
  this.bufImg1 = new Image();
  this.bufImg2 = new Image();
  this.newImg = this.bufImg1;
  this.myID = myid;
  this.GetDmsImgStr = function(w, h)
  {
    //20070613 Luther fix 1280 * 960
    if (w > 720)
    {
      h = (720 * h) / w;
      w = 720;
    }
    return '<img src="" width="' + w + '" height="' + h + '" id="showdms_' + this.myID + '" border=0 >';
  };

  this.imgSwitch = function()
  {
    if (this.newImg == this.bufImg1)
    {
      this.newImg = this.bufImg2;
    }
    else
    {
      this.newImg = this.bufImg1;
    }
  };

  this.RunDms = function()
  {
    if (jpegTimer == null)
    {
      jpegTimer = setTimeout("JpegFrameCal()", 1000);
    }
    g_dmsRun = true;
    // this.newImg = new Image();
    this.imgSwitch();
    // var d = g_dmsList[id];
    var obj = GE("showdms_" + this.myID);
    if (obj != null)
    {
      this.newImg.id = "showdms_" + this.myID;
      this.newImg.onload = DmsOK;
      this.newImg.src = "/dms?" + Math.random();
    }
  };

};
function DmsOK()
{
  //id = 0;
  var z = 1;
  if (GE("zoomSel") != null)
  {
    z = parseInt(GetValue("zoomSel"));
  }
  //var obj = GE("showdms_"+this.myID);
  var obj = GE("showdms_0");
  if (obj != null)
  {
    var p = obj.parentNode;
    p.removeChild(obj);
    var b = imgFetcher.newImg;
    if (CONTENT_PAGE == "main.htm")
    {
      b.width = g_viewXSize * z;
      b.height = g_viewYSize * z;
    }
    else
    {
      b.width = obj.width;
      b.height = obj.height;
    }
    p.appendChild(b);
    // alert("g_dmsRun="+g_dmsRun+" g_lockLink="+g_lockLink);
    if (g_dmsRun && !g_lockLink)
      setTimeout("imgFetcher.RunDms()", 5);
    jpegCounter++;
  }

};

var imgFetcher = new ImageFetcher(0);
g_fetchList[0] = imgFetcher;
function JpegFrameCal()
{
  WS(jpegCounter + " fps");
  if (jpegCounter == 0)
  {
    zeroFpsCount++;
  }
  else
  {
    zeroFpsCount = 0;
  }
  jpegCounter = 0;
  if (g_dmsRun && !g_lockLink)
  {
    jpegTimer = setTimeout("JpegFrameCal()", 1000);
  }
  if (zeroFpsCount >= 10)
  {
    zeroFpsCount = 0;
    // this is not good.
    // alert("RESTART DMS");
    var n;
    for (n in g_fetchList)
    {
      var obj = GE("showdms_" + n);
      // alert("n="+n+" obj="+obj);
      if (obj != null)
      {
        g_fetchList[n].RunDms();
      }
    }

  }
};

function InitLoad()
{
  var datekey = (new Date()).getTime();
  loadJS("setInnerHTML.js");
  loadJS("lang_" + g_langName + ".jsl?" + datekey);
  loadJS("override.js");
};

// you must prepare a <div> element,
// ex: <div id="pppoe_view" style="position:absolute; top:0;left:0;width:0;height:0; z-index:3; visibility:hidden"></div>
//
function PopView(divName, x, y, w, h, url)
{
  var obj = document.getElementById(divName);
  obj.style.top = y;
  obj.style.left = x;
  obj.innerHTML = '<table ><tr><td style="border-width:thick;border-color:red"><iframe width=' + w + ' height=' + h + ' src="' + url + '" ></iframe></td></tr><tr><td align="center"><input name="PopClose" type="button" value="Close" onClick=PopViewClose("' + divName + '")></td></tr></table>';
};

function PopViewClose(divName)
{
  document.getElementById(divName).style.visibility = 'hidden';
};
function PopViewCenter(divName, w, h, url)
{
  var x = (document.body.offsetWidth / 2) - (w / 2);
  var y = (document.body.offsetHeight / 2) - (h / 2);
  PopView(divName, x, y, w, h, url);
};

// 20070419 Luther Add ---START---
// add change channel then change the title.
function ChangeChannelTextPanel(id)
{
  var obj = GE("chtxt");
  if (obj != null)
  {
    if (id <= 0)
    {
      id = 1;
    }
    obj.innerHTML = GL("channel") + " " + id;
  }
};
function GetChannelTextPanelHTML(id)
{
  if (id <= 0)
  {
    id = 1;
  }
  return "<span id='chtxt'>" + GL("channel") + " " + id + "</span>";
};
// 20070419 Luther add --- END ---

// 20070620 Luther Add ---START---
// add change channel then change the title.
function IsEarlyIpCam()
{
  return (g_machineCode == "1688" || // 7214
  g_machineCode == "1788" || // 7215
  g_machineCode == "5678" || // 7211,7221
  g_machineCode == "5679"); // 7211W
};

// 20070620 Luther add --- END ---
// 20080204 Luther add ---START---
function GetCookie(name)
{
  var cname = name + "=";
  var dc = document.cookie;
  if (dc.length > 0)
  {
    begin = dc.indexOf(cname);
    if (begin != -1)
    {
      begin = begin + cname.length;
      end = dc.indexOf(";", begin);
      if (end == -1)
      {
        end = dc.length;
      }
      return dc.substring(begin, end);
    }
  }
  return null;
};

function SetCookie(name, value, expires)
{
  CookiesExpDay = "365";
  var time = new Date();
  time.setTime(time.getTime() + ((86400000) * CookiesExpDay));
  if (expires == null)
  {
    expires = "";
  }
  document.cookie = name + "=" + value + ";expires=" + time.toGMTString();
};
function SaveCombo(id, cookieName)
{
  var obj = GE(id);
  if (obj != null)
  {
    var ix = obj.selectedIndex;
    if (ix >= 0)
    {
      SetCookie(cookieName, obj.options[obj.selectedIndex].value);
    }
  }
};
function GetCookieInt(name, def)
{
  var i = parseInt(GetCookie(name));
  if (isNaN(i))
  {
    i = def;
  }
  return i;
};

// 20080204 Luther add --- END ---
// 20080205 Luther add ---START---
// add safe function call
function SafeCall(strFun)
{
  //if (typeof(eval(strFun)) != "undefined")
  if (eval("typeof " + strFun + " != 'undefined'"))
  {
    eval(strFun + "()");
  }
};

// 20080205 Luther add --- END ---
// 20080216 Luther add ---START---
// copy from internet, get the
function Point(_x, _y)
{
  this.x = _x;
  this.y = _y;
};
function RealPosition(_obj)
{
  var currPos = new Point(_obj.offsetLeft, _obj.offsetTop);
  var workPos = new Point(0, 0);
  if (_obj.offsetParent.tagName.toUpperCase() != "BODY")
  {
    workPos = RealPosition(_obj.offsetParent);
    currPos.x += workPos.x;
    currPos.y += workPos.y;
  }
  return currPos;
};
// 20080216 Luther add --- END ---

// return the count of space, &nbsp;...
function Space_(c)
{
  var o = new StringBuffer();
  for ( var i = 0; i < c; i++)
  {
    o.Add("&nbsp;");
  }
  return o.ToString();
};

function GetBaseXY()
{
  var obj = GE("theObjTable");
  if (obj != null)
  {
    var p = RealPosition(obj);
    baseY = p.y;
    baseX = p.x;
  }
};

// ======IMAGE=======
// SCR : Submit Change Right now.
function SCR(key)
{
  var obj = CTRLARY[key];
  if (obj != null)
  {
    SendHttp(c_iniUrl + GetSetterCmd(obj, GetCCV(key)), false);
  }
};
function CheckBackLight()
{
  var vv;
  if (s_blcmode == 1)
  {
    SCR("blcmode");
    vv = GetCCV("blcmode");
    // vv = 0 ,1 disable it.
    DisableImgProp("backlight", vv <= 1);
  }
  else if (s_blc == 1)
  {
    SCR("blc");
    vv = GetCCV("blc");
    // vv = 0 disable it.
    DisableImgProp("backlight", vv == 0);
  }
};
function DisableImgProp(n, isDis)
{
  GE(n).disabled = isDis;
  GE("IBN_" + n).disabled = isDis;
  GE("DBN_" + n).disabled = isDis;
};
// isNL : add new line before range text
function ImgEditor_(key, isNL)
{
  var o = '';
  if (key != null)
  {
    o += '<input id=DBN_' + key + ' type=button value="<" style="height:20px;width:20px" notabstop onClick="ImgSetDecJump(\'' + key + '\',8);" onDblClick="ImgSetDecJump(\'' + key + '\',32);">';
    o += WH_(key);
    o += '<input id=IBN_' + key + ' type=button value=">" style="height:20px;width:20px" notabstop onClick="ImgSetIncJump(\'' + key + '\',8);" onDblClick="ImgSetIncJump(\'' + key + '\',32);">';
    if (isNL)
    {
      o += '<br>';
    }
    o += '(0~255)';
  }
  return o;
};
// restore date back to Data Content List.
function SaveDCL()
{
  if (IsVS() && g_CHID > 0)
  {
    var dcl = eval("DCL_" + g_CHID);
    if (s_bright == 1)
      dcl.brightness = GetCCV("brightness");
    if (s_contrast == 1)
      dcl.contrast = GetCCV("contrast");
    if (s_sat == 1)
      dcl.saturation = GetCCV("saturation");
    if (s_hue == 1)
      dcl.hue = GetCCV("hue");
  }
};
function ImgSetDecJump(key, gap)
{
  var ctrl = CTRLARY[key];
  if (ctrl != null && ctrl.IsPass())
  {
    var v = parseInt(GetCCV(key)) - gap;
    if (v < 0)
      v = 0;
    SetCCV(key, v);
    SaveDCL();
    SCR(key);
  }
};
function ImgSetIncJump(key, gap)
{
  var ctrl = CTRLARY[key];
  if (ctrl != null && ctrl.IsPass())
  {
    var v = parseInt(GetCCV(key)) + gap;
    if (v > 255)
      v = 255;
    SetCCV(key, v);
    SaveDCL();
    SCR(key);
  }
};
// mPage: main Page conaint HTML ex: lang.htm
// cPage: content Page, ex: c_lang
function SafeCallPage(mPage, cPage)
{
  //fix cms bug.
  if (THIS_PAGE.indexOf("cms.htm") == 0)
  {
    //add other variable
    var ix = THIS_PAGE.indexOf("&");
    var ex = "";
    if (ix > 0)
    {
      ex = THIS_PAGE.substring(ix);
    }
    location.href = "cms.htm?" + cPage + ex;
  }
  else
  {
    location.href = mPage;
  }
};

function UpdateGSizeLite()
{
  var z = GetLiteStreamNum();
  g_viewXSize = eval("g_s"+z+"XSize");
  g_viewYSize = eval("g_s"+z+"YSize");
}


function GetFirstShowMode()
{
  //for alignment so break the format
  if (g_isSupS5) return 5;
  if (g_isSupS2) return 2;
  if (g_isSupS1) return 1;
  if (g_isSupS6) return 6;
  if (g_isSupS3) return 3;
  if (g_isSupS4) return 4;
};

function GetLiteStreamNum()
{
  if (g_isSupS1) return 1;

  if (g_isSupS3) return 3;

  if (g_isSupS6) return 6;
  if (g_isSupS2) return 2;
  if (g_isSupS5) return 5;
  if (g_isSupS4) return 4;
}

function Get9001FirstCodec(sel)
{
  if (isNaN(sel)) {
    sel = GetLiteStreamNum();
  }
  switch(sel)
  {
  case 2:
  case 3:
    return V_MPEG4;
  case 5:
  case 6:
    return V_H264;
  //case 1:
  //case 4:
  default:
    return V_JPEG;
  }
}
