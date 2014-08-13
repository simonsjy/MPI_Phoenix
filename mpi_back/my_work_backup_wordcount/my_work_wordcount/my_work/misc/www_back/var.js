function GV(v, df)
{
  return (v.indexOf("<%") > -1) ? df : v;
}
var g_isSupMpeg4 = GV("<%supportmpeg4%>", 1);
var g_videoFormat = GV("<%format%>", 0);
var g_isPal = (GV("<%imagesource%>", 1) == "1");
var g_deviceName = GV("<%devicename%>", "VideoServer");
var g_defaultStorage = GV("<%defaultstorage%>", 1); // 0: cf, 1:sd 255:no card
var g_SDInsert = GV("<%sdinsert%>", 0);
var g_CFInsert = GV("<%cfinsert%>", 0);
var g_cardGetLink = GV("<%defaultcardgethtm%>", "sdget.htm");
var g_brandUrl = GV("<%brandurl%>", null);
var g_titleName = GV("<%title%>", "IPCAM");
var g_brandName = "nobrand";
var g_supportTStamp = GV("<%supporttstamp%>", 0);
var g_mpeg4XSize = 0;
var g_mpeg4YSize = 0;
var g_jpegXSize = 0;
var g_jpegYSize =0;
var g_socketAuthority = parseInt(GV("<%socketauthority%>", 3)); // 0:admin,1:operator,2:viewer
var g_isAuthorityChange = (parseInt(GV("<%authoritychange%>", 0)) == 1);
var g_isSupMotion = (parseInt(GV("<%supportmotion%>", 0)) >= 1);
var g_isSupWireless = (parseInt(GV("<%supportwireless%>", 0)) == 1);
var g_serviceFtpClient = parseInt(GV("<%serviceftpclient%>", 0));
var g_serviceSmtpClient = parseInt(GV("<%servicesmtpclient%>", 0));
var g_servicePPPoE = parseInt(GV("<%servicepppoe%>", 0));
var g_serviceSNTPClient = parseInt(GV("<%servicesntpclient%>", 0));
var g_serviceDDNSClient = parseInt(GV("<%serviceddnsclient%>", 0));
var g_s_maskarea = GV("<%supportmaskarea%>", 0);
var g_machineCode = "<%machinecode%>";
var g_maxCH = GV("<%maxchannel%>", 4);
var g_isSupportRS485 = ("<%supportrs485%>" == "1");
var g_isSupportRS232 = ("<%supportrs232%>" == "1");
var g_useActiveX = GV("<%layoutnum.0%>", 1);
var g_ptzID = GV("<%layoutnum.1%>", 1);
var g_s_mui = GV("<%supportmui%>", 1);
var g_mui = GV("<%mui%>", -1);
var g_isSupportSeq = ("<%supportsequence%>" == "1");
var g_isSupportMQ = (parseInt("<%quadmodeselect%>") >= 0);
var g_quadMode = GV("<%quadmodeselect%>", 1); // default is 1:quad
var g_isSupportSmtpAuth = true;
var g_isSupportIPFilter = ("<%serviceipfilter%>" == "1");
var g_oemFlag0 = (parseInt(GV("<%oemflag0%>", 0)));
var g_s_daynight = (parseInt(GV("<%supportdncontrol%>", 0)) == 1);
var g_is264 = (parseInt(GV("<%supportavc%>", 0)) == 1);
var g_isSupportD2N = false;
var g_isSupportN2D = false;
var g_isSupportAudio = (parseInt(GV("<%supportaudio%>", 0)) >= 1);
var g_isSelMpeg4 = (parseInt(GV("<%supportavc%>", 0)) == 1);
var g_motionxblock = (parseInt(GV("<%motionxblock%>", 0)));
var g_motionyblock = (parseInt(GV("<%motionyblock%>", 0)));
var g_motionblock = GV("<%motionblock%>", 000);
//==========================
// PTZ Control
//==========================
// supportptzpage 0:NO PTZ, 1:NORMAL, 2:DV840, 3:DV840 with Full Control 4:New
// UI for Peloco-D
// Note: old machine no this command, so default is 1
var g_isSupportPtz = (parseInt(GV("<%supportptzpage%>", 1)) > 0);
// display old PTZ control
var g_isShowOldPtzPage = (parseInt(GV("<%supportptzpage%>", 1)) == 1);
// var g_isShowPtzCtrl=((g_machineCode=="1670") && (g_socketAuthority < 2));
var g_isShowPtzCtrl = false;
var g_isShowFullPtzCtrl = false;
var g_isShowPtzPD = false;

// LUTHER TEST FOR 7242
// var g_isSupportPtz=true;
// var g_isShowOldPtzPage=false;
// var g_isShowPtzCtrl=false;
// var g_isShowFullPtzCtrl=false;
// var g_isShowPtzPD=true;

var g_isShowDvPtzCtrl = (g_isShowPtzCtrl || g_isShowFullPtzCtrl || g_isShowPtzPD);

//==========================
// Multi Profile
//==========================
var g_s1XSize = parseInt(GV("<%stream1xsize%>", 320));
var g_s1YSize = parseInt(GV("<%stream1ysize%>", 240));
var g_s2XSize = parseInt(GV("<%stream2xsize%>", 320));
var g_s2YSize = parseInt(GV("<%stream2ysize%>", 240));
var g_s3XSize = parseInt(GV("<%stream3xsize%>", 320));
var g_s3YSize = parseInt(GV("<%stream3ysize%>", 240));
var g_s4XSize = parseInt(GV("<%stream4xsize%>", 320));
var g_s4YSize = parseInt(GV("<%stream4ysize%>", 240));
var g_s5XSize = parseInt(GV("<%stream5xsize%>", 320));
var g_s5YSize = parseInt(GV("<%stream5ysize%>", 240));
var g_s6XSize = parseInt(GV("<%stream6xsize%>", 320));
var g_s6YSize = parseInt(GV("<%stream6ysize%>", 240));
var g_isSupS1 = (parseInt(GV("<%supportstream1%>", 0)) >= 1);
var g_isSupS2 = (parseInt(GV("<%supportstream2%>", 0)) >= 1);
var g_isSupS3 = (parseInt(GV("<%supportstream3%>", 0)) >= 1);
var g_isSupS4 = (parseInt(GV("<%supportstream4%>", 0)) >= 1);
var g_isSupS5 = (parseInt(GV("<%supportstream5%>", 0)) >= 1);
var g_isSupS6 = (parseInt(GV("<%supportstream6%>", 0)) >= 1);

var g_isSupportMultiProfile = true;
var g_isSupportQuickSet = false;
var g_isMP1 = true;
var g_isMP73 = false;

var g_isEventApp = false;

function loadJS(url)
{
  document
      .write('<sc' + 'ript language="javascript" type="text/javascript" src="' + url + '"></script>');
}
