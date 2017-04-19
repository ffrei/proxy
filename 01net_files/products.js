/*
 * Common .js for all weborama products
 *
 */

/* ======================== common ======================== */
function _ap_defined(name) {
    return ( typeof(window[name]) == "undefined" ) ? false : true;
}
function wis_defined(name) {
     return (typeof(name) == 'undefined' || name == null) ? false : true;
}

/* cookies */
function getCookieVal (offset) {
	var endstr = document.cookie.indexOf (";", offset);
    if (endstr == -1)
        endstr = document.cookie.length;
	return unescape(document.cookie.substring(offset, endstr));
}

function GetCookie (name) {
	var arg = name + "=";
	var alen = arg.length;
	var clen = document.cookie.length;
	var i = 0;
	while (i < clen) {
		var j = i + alen;
		if (document.cookie.substring(i, j) == arg)
			return getCookieVal (j);
		i = document.cookie.indexOf(" ", i) + 1;
		if (i == 0) break;
	}
    return null;
}

function SetCookie (name,value,expires,path,domain,secure)  {
	document.cookie = name + "=" + escape (value) +
	((expires) ? "; expires=" + expires.toGMTString() : "") +
	((path) ? "; path=" + path : "") +
	((domain) ? "; domain=" + domain : "") +
	((secure) ? "; secure" : "");
}

/* string */
function encode_en_lettre (num) {
	num = parseInt(num,10);
	if(num > 2500) return '';
	var num1 = parseInt(num/52,10);
	var num2 = num % 52;

	num1 += 65;
	if (num1>90) num1+=6;

	num2 += 65;
	if (num2>90) num2+=6;

	var mon_code52 = String.fromCharCode(num1) + String.fromCharCode(num2);
	return mon_code52;
}

function traite_chaine(str,taille_max) {
	if (str == null) return null;
	var s = traduction(str);
	var bag = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-.,;:_ ";
	var i;
	var returnString = "";
	if (s == null) return "";
	s = "" + s;
	s = s.substr(0,taille_max);
	for (i = 0; i < s.length; i++) {
		var c = s.charAt(i);
		if (bag.indexOf(c) != -1) returnString += c;
	}
	returnString = unicite_espace(returnString);
	returnString = escape(returnString);
	return returnString;
}

function convertir(chaine) {
	var str = chaine.replace(/&#(\d+);/,"$1");
	return String.fromCharCode(str);
}

function traduction(chaine) {
	var chaine_b = unescape(chaine);
	var i=0;
	while ( chaine_b != chaine && i<5 )
	{
		i++;
		chaine = chaine_b;
		chaine_b = unescape(chaine_b);	
	}
	return chaine_b.replace(/&#(\d+);/gi, convertir);
}

function unicite_espace(chaine) {
	var str = chaine.replace(/(\s+)/g,' ');
	return str.replace(/(^\s*)|(\s*$)/g,"");
}

// Identification du type de navigateur
function wf_uaO(c) {
	var wf_ual=navigator.userAgent.toLowerCase();
	return(wf_ual.indexOf(c))
}


/* ======================== analytics ======================== */
_NB_MAX_CONTENU_ 	= 1;
_TAILLE_MAX_CONTENU_ 	= 100;
_TAILLE_MAX_CHAINE_ 	= 50;
_TAILLE_MAX_ALPHANUM_ 	= 30;
_COOKIE_SEGMENTATION    = 'wbo_segment_';
var _NB_MAX_EXTEND_PARAMETERS = 5;
var _TAILLE_MAX_EXTEND_PARAMETER_ = 100;

var _ap_an         = new Object;
_ap_an.wrp_host        = ( _ap_defined('WRP_HOST') ) ? WRP_HOST : 'pro.weborama.fr';
_ap_an.wrp_host_ssl    = ( _ap_defined('WRP_HOST_SSL') ) ? WRP_HOST_SSL : _ap_an.wrp_host;
_ap_an.site_grp    = ( _ap_defined('WRP_ID_GRP') ) ? WRP_ID_GRP : null;
_ap_an.site        = ( _ap_defined('WRP_ID') ) ? WRP_ID : null;
_ap_an.section     = ( _ap_defined('WRP_SECTION') ) ? WRP_SECTION : null;
_ap_an.subsection  = ( _ap_defined('WRP_SUBSECTION') ) ? WRP_SUBSECTION : null;
_ap_an.section_grp = ( _ap_defined('WRP_SECTION_GRP') ) ? WRP_SECTION_GRP : null;
_ap_an.subsection_grp = ( _ap_defined('WRP_SUBSECTION_GRP') ) ? WRP_SUBSECTION_GRP : null;
_ap_an.frame  	   = ( _ap_defined('WRP_ACC') ) ? WRP_ACC : null;
_ap_an.channel     = ( _ap_defined('WRP_CHANNEL') ) ? WRP_CHANNEL : null;
_ap_an.content     = ( _ap_defined('WRP_CONTENT') ) ? WRP_CONTENT : null;
_ap_an.wrp_profiling_mode = ( _ap_defined('WRP_PROFILING_COOKIE_MODE') ) ? WRP_PROFILING_COOKIE_MODE : 1;

_ap_an.wreport_counter = function()
{
	if (this.site == null || this.section == null || this.subsection == null)  return 1;

	this.nb_content = 0;
	this.url = '';
	this.segmentation = new Array(5);	
	this.profiles = '';

	this.cookie_segmentation = _COOKIE_SEGMENTATION+this.site;
	this.domaine_segmentation = '';

	this.host 	= this.wrp_host + '/fcgi-bin/comptage_wreport.fcgi';
	this.host_ssl 	= this.wrp_host_ssl + '/fcgi-bin/comptage_wreport.fcgi';

	this.extendparameters = '';
	this.extend_parameters = new Array(_NB_MAX_EXTEND_PARAMETERS);

	/* Initialisation */
	this.section = traite_chaine(this.section,_TAILLE_MAX_ALPHANUM_);
	this.subsection = traite_chaine(this.subsection,_TAILLE_MAX_ALPHANUM_);
	this.chaine = traite_chaine(this.chaine,_TAILLE_MAX_CHAINE_)
	this.section_grp = traite_chaine(this.section_grp,_TAILLE_MAX_ALPHANUM_);
	this.subsection_grp = traite_chaine(this.subsection_grp,_TAILLE_MAX_ALPHANUM_);
        this.channel = traite_chaine(this.channel,_TAILLE_MAX_CHAINE_);

	/* groupe */
	if (this.site_grp == null || this.section_grp == null || this.subsection_grp == null) {
	   this.site_grp = null;
	   this.section_grp = null;
	   this.subsection_grp = null;
	}

	this.counter = new Image(1,1);
	this.generate_url();
	this.counter.src = this.url;

	return 0;
}

// Evolution #38044: Activate bigsea call for a list of FR crawlable accounts
_ap_an.bigsea_rd = function()
{
    var protocole = 'http:';
    if ( location.protocol == 'https:') protocole = "https:";
    this.bs = new Image(1,1);
    this.bs.src = protocole + '//bigsea.solution.weborama.fr/fcgi-bin/dispatch.fcgi?d.A=rd&d.u=' + escape( protocole + '//dx.bigsea.weborama.com/collect?dsp_id=0&eid={WEBO_ID}&touchpoint=0&url=' + escape( document.location ) );
    return 0;
}

// Génére l'url de comptage
_ap_an.generate_url = function()
{
	var _date_ = new Date();
	this.date = parseInt(_date_.getTime()/1000 - 60*_date_.getTimezoneOffset());
	this.ref = ''+escape(document.referrer);

	this.ta = '0x0';
	this.co = 0;
	this.nav = navigator.appName;

	this.get_profiles();
	this.get_extend_parameters();

	if ( parseInt(navigator.appVersion)>=4)
	{
  		this.ta = screen.width+"x"+screen.height;
		this.co = (this.nav!="Netscape")?screen.colorDepth:screen.pixelDepth;
	}
        if ((this.frame != null)&&(this.nav!="Netscape"))
        {
    	   var reftmp = 'parent.document.referrer';
           if((this.frame<5)&&(this.frame>0)) { for(_k=this.frame;_k>1;_k--) reftmp = 'parent.' + reftmp;}
           var mon_ref = eval(reftmp);
           if(document.referrer == parent.location || document.referrer=='') this.ref=''+escape(mon_ref);
        }

	this.url = "http://"+this.host+"?WRP_ID="+this.site;
	if ( location.protocol == 'https:') this.url = "https://"+this.host_ssl+"?WRP_ID="+this.site;

	if(this.profiles != null)  this.url += "&WRP_PFL="+escape(this.profiles);
	if(this.extendparameters != null)  this.url += this.extendparameters;

	var is_mac=(wf_uaO('mac')!=-1);
	var is_opera=(wf_uaO('opera')!=-1);
	if((!is_mac)&&(!is_opera)) {
		var msieind=navigator.userAgent.indexOf('MSIE');
		if(msieind>0) {
  		if(parseInt(navigator.userAgent.charAt(msieind+5))>=5) {
				document.body.addBehavior("#default#clientCaps");
				this.cnx = (document.body.connectionType == 'modem') ? 'A':'B';
				document.body.addBehavior("#default#homePage");
				this.home = (document.body.isHomePage(location.href)) ? 'A':'B';
				this.url+="&CONN="+this.cnx+"&ISHOME="+this.home;
			} 
		}
	} 
	this.url += "&WRP_SECTION="+this.section+"&WRP_SUBSECTION="+this.subsection;

	if(this.site_grp != null && this.section_grp != null && this.subsection_grp != null)
        {
		this.url+="&WRP_ID_GRP="+this.site_grp+"&WRP_SECTION_GRP="+this.section_grp+"&WRP_SUBSECTION_GRP="+this.subsection_grp;
	}

	if(this.content != null) this.url+="&WRP_CONTENT="+this.content;
	if(this.channel != null) this.url+="&WRP_CHANNEL="+this.channel;

    this.url += "&ver=2&da2="+this.date+"&ta="+this.ta+"&co="+this.co+"&ref="+this.ref;
}

/* ------------------ Profilling ------------------------ */

// Ajout d'un profil pour le profiling
_ap_an.add_profile = function(numero,valeur)
{
	numero = parseInt(numero,10);
	if ((numero < 1) || (numero > 5)) return -2;
	if ( this.wrp_profiling_mode == 0 )
	{
		numero--;
		this.segmentation[numero] = valeur;
		return 1;
	}
	if(parseInt(navigator.appVersion,10)<=3) return -1;
	nb_mois = 12;
	var verif_val_I = /^\d+$/;
	if(verif_val_I.test(valeur)) valeur = encode_en_lettre(valeur);
	var verif_val_A = /^\w*$/;
	if(verif_val_A.test(valeur)) {
		var mon_profil_wbo = GetCookie(this.cookie_segmentation);
		tab_segment = new Array('','','','','');
		if(mon_profil_wbo != null) {
			tab_segment = mon_profil_wbo.split('|');
			if(tab_segment.length != 5) tab_segment = ('','','','','');
				//On vérifie qu'une mise à jour est nécessaire.
			if(tab_segment[numero - 1]==valeur) return 1;
		}
		if(this.domaine_segmentation == '') {
			this.domaine_segmentation = window.location.host;
			tab_points = new Array();
			tab_points = window.location.host.split('.');
			if(tab_points.length>2) this.domaine_segmentation = this.domaine_segmentation.substring(this.domaine_segmentation.indexOf('.'),this.domaine_segmentation.length);
			if(tab_points.length==2) this.domaine_segmentation = '.' + this.domaine_segmentation;
		}
		var ma_chaine_profil='';
		for(var i=1; i<=5; i++) {
			if(i==numero) tab_segment[i-1]=valeur;
			if((tab_segment[i-1]=='') || (tab_segment[i-1]==null)) tab_segment[i-1] = '';
			ma_chaine_profil += tab_segment[i-1];
			if(i<5) ma_chaine_profil += '|';
		}
		expd = new Date();
		expd.setTime(expd.getTime() + (nb_mois * 30 * 24 * 3600 * 1000));
		SetCookie(this.cookie_segmentation,ma_chaine_profil,expd,'/',this.domaine_segmentation);
		return 2;
	}
	else return -3;
}

// Ajout du profiling
_ap_an.add_profiles = function(p1,p2,p3,p4,p5) {
	this.add_profile(1,p1);
	this.add_profile(2,p2);
	this.add_profile(3,p3);
	this.add_profile(4,p4);
	this.add_profile(5,p5);
}

// Destruction du cookie de profiling ou de la variable
_ap_an.delete_profiles = function()
{
	if ( this.wrp_profiling_mode == 0 )
	{
		this.segmentation = Array('','','','','');
	}
	else
	{
		expd = new Date();
		expd.setTime(expd.getTime() - (24 * 3600 * 1000));
		SetCookie (this.cookie_segmentation,'||||',expd,'/',this.domaine_segmentation);
	}
}

// Recuperation des informations de profiling ( on renvoie une chaine escapée )
_ap_an.get_profiles = function() {
	this.profiles = GetCookie(this.cookie_segmentation);
	if( this.profiles == null) {
		this.profiles = '';
		var verif_val_I = /^\d+$/;
                var is_def = 0;
		for(var i = 0 ;i <= 4; i++) {
			if(verif_val_I.test(this.segmentation[i])) this.segmentation[i] = encode_en_lettre(this.segmentation[i]);
			if (wis_defined(this.segmentation[i])) { this.profiles += this.segmentation[i]; is_def = 1; }
			if(i < 4) this.profiles += '|';
		}
		if (is_def == 0) this.profiles = null; 
	}
}


/* ------------------ EXTEND PARAMETERS -------------------- */

_ap_an.add_extend_parameters = function(p1,p2,p3,p4,p5) {
	this.add_extend_parameter(1,p1);
	this.add_extend_parameter(2,p2);
	this.add_extend_parameter(3,p3);
	this.add_extend_parameter(4,p4);
	this.add_extend_parameter(5,p5);
}

_ap_an.add_extend_parameter = function(numero,valeur) {
	numero = parseInt(numero,10);
	if ((numero < 1) || (numero > _NB_MAX_EXTEND_PARAMETERS)) return -2;
	this.extend_parameters[--numero] = clean_extend_parameter(valeur);
}

_ap_an.clean_extend_parameter = function(s) {
	if (s == null) return "";
	s = "" + s;
	return s.substr(0,_TAILLE_MAX_EXTEND_PARAMETER_);
}

_ap_an.get_extend_parameters = function() {
	this.extendparameters = '';
	for(var i=1;i <= _NB_MAX_EXTEND_PARAMETERS; i++) {
		if (wis_defined(this.extend_parameters[i-1])) this.extendparameters += "&BI"+i+"="+encodeURIComponent(this.extend_parameters[i-1]);
	}	
}


function create_ifrtrk() {
    var ifrurl='//cstatic.weborama.fr/iframe/external_all.html';
	if ( location.protocol == 'https:') ifrurl = "https:"+ifrurl;
    else ifrurl = "http:"+ifrurl;

    try {
        var wbo_ifrm = document.createElement('IFRAME'), bodyRef = document.getElementsByTagName('body').item(0) || document.documentElement.childNodes[0];
        wbo_ifrm.style.width=wbo_ifrm.style.height='1px';
        wbo_ifrm.style.border='0px';
        wbo_ifrm.style.position='absolute';
        wbo_ifrm.style.display='none';
        wbo_ifrm.style.top=wbo_ifrm.style.left='0px';
        wbo_ifrm.style.zIndex=0;
        wbo_ifrm.src=ifrurl;
        if (bodyRef.firstChild != null) {
            bodyRef.insertBefore(wbo_ifrm, bodyRef.firstChild);
        } else {
            bodyRef.appendChild(wbo_ifrm);
        }
    } catch (e) {
        document.write("<iframe src='"+ifrurl+"' width=1 height=1 frameborder=0 style='border:0px'><\/iframe>");
    }
}

// main
_ap_an.wreport_counter();
// Evolution #38044: Activate bigsea call for a list of FR crawlable accounts
// Evolution #38973: Push most of the WAI traffic to BigSea using a static list in the JS
var acc_list = {484801:1,428200:1,485128:1,389168:1,485086:1,485085:1,261612:1,294353:1,333681:1,452526:1,326755:1,333514:1,427639:1,340158:1,349614:1,391591:1,402654:1,380783:1,382236:1,414184:1,427616:1,445081:1,427633:1,454082:1,445589:1,448983:1,456573:1,461323:1,462385:1,466169:1,485084:1,478139:1,469355:1,468315:1,468316:1,468317:1,468318:1,468319:1,468396:1,468397:1,468867:1,469166:1,485163:1,469354:1,469341:1,469342:1,469343:1,469347:1,469350:1,469351:1,469353:1,469356:1,469422:1,469423:1,469424:1,469491:1,469710:1,469711:1,469712:1,469713:1,469714:1,469718:1,470044:1,470180:1,470182:1,470348:1,470617:1,470809:1,471046:1,471253:1,471254:1,471255:1,471256:1,471257:1,471258:1,471357:1,471624:1,471781:1,471784:1,471788:1,471789:1,471790:1,471791:1,471793:1,471794:1,471795:1,471796:1,471797:1,471798:1,471839:1,485034:1,473971:1,472274:1,472319:1,472551:1,472553:1,472816:1,472820:1,472843:1,473545:1,473031:1,473544:1,473543:1,473615:1,473616:1,473617:1,473618:1,473619:1,473620:1,473621:1,476205:1,473772:1,473855:1,474315:1,474759:1,474760:1,474761:1,474856:1,474857:1,474858:1,474859:1,474860:1,474861:1,474963:1,474964:1,474966:1,474967:1,474970:1,475082:1,475083:1,475084:1,475085:1,475086:1,475088:1,485126:1,475310:1,475353:1,475482:1,475494:1,475777:1,475825:1,475971:1,475973:1,476042:1,476218:1,476368:1,476378:1,476382:1,476383:1,476384:1,476482:1,476484:1,476485:1,476486:1,476527:1,476576:1,476577:1,476578:1,476685:1,484884:1,484808:1,478392:1,478563:1,478564:1,478585:1,478716:1,479438:1,479819:1,479821:1,479866:1,480284:1,480215:1,480300:1,480301:1,480395:1,480832:1,480836:1,480942:1,480943:1,481213:1,481214:1,481241:1,481322:1,481503:1,481515:1,481660:1,481716:1,481941:1,481925:1,482113:1,482721:1,482722:1,482723:1,482725:1,482726:1,482727:1,482732:1,482770:1,482869:1,482936:1,482937:1,482938:1,482988:1,483042:1,483043:1,484878:1,483639:1,483922:1,484296:1,484297:1,484413:1,484372:1,484410:1,484467:1,484817:1,484572:1,484636:1,484690:1,484692:1,485122:1,485148:1,484802:1,484803:1,484804:1,484805:1,484806:1,484809:1,484811:1,484818:1,484833:1,484834:1,484835:1,484836:1,484837:1,484838:1,484839:1,484840:1,484841:1,484842:1,484843:1,484859:1,484860:1,484861:1,484862:1,484926:1,485074:1,484871:1,485155:1,484879:1,484880:1,484881:1,484882:1,484883:1,484888:1,484885:1,484894:1,484904:1,485246:1,484949:1,484895:1,484896:1,484897:1,484901:1,484934:1,485401:1,484980:1,484902:1,484903:1,484930:1,484931:1,484932:1,485135:1,485053:1,484964:1,484984:1,484950:1,484951:1,484954:1,484957:1,484958:1,484960:1,484963:1,484966:1,484967:1,484968:1,484971:1,484972:1,485054:1,485230:1,485017:1,484981:1,484982:1,484985:1,485076:1,485223:1,485016:1,485015:1,485012:1,485018:1,485033:1,485035:1,485061:1,485060:1,485058:1,485127:1,485256:1,485069:1,485071:1,485077:1,485078:1,485079:1,485080:1,485081:1,485087:1,485089:1,485090:1,485091:1,485092:1,485093:1,485094:1,485095:1,485096:1,485097:1,485098:1,485099:1,485100:1,485101:1,485102:1,485103:1,485104:1,485105:1,485106:1,485107:1,485108:1,485109:1,485110:1,485111:1,485112:1,485113:1,485114:1,485115:1,485116:1,485117:1,485118:1,485119:1,485120:1,485121:1,485131:1,485132:1,485133:1,485134:1,485138:1,485139:1,485183:1,485156:1,485143:1,485144:1,485146:1,485147:1,485149:1,485161:1,485153:1,485171:1,485157:1,485167:1,485160:1,485261:1,485209:1,485164:1,485165:1,485168:1,485169:1,485170:1,485248:1,485174:1,485175:1,485176:1,485177:1,485178:1,485181:1,485179:1,485180:1,485182:1,485184:1,485185:1,485186:1,485188:1,485292:1,485190:1,485191:1,485192:1,485221:1,485301:1,485210:1,485211:1,485212:1,485213:1,485214:1,485216:1,485217:1,485218:1,485219:1,485220:1,485237:1,485228:1,485229:1,485231:1,485307:1,485266:1,485235:1,485236:1,485254:1,485238:1,485239:1,485240:1,485241:1,485242:1,485243:1,485244:1,485262:1,485257:1,485247:1,485250:1,485285:1,485255:1,485259:1,485283:1,485325:1,485279:1,485263:1,485264:1,485265:1,485267:1,485268:1,485269:1,485270:1,485271:1,485272:1,485273:1,485274:1,485275:1,485276:1,485277:1,485278:1,485280:1,485281:1,485282:1,485286:1,485288:1,485289:1,485290:1,485291:1,485298:1,485297:1,485294:1,485296:1,485299:1,485331:1,485302:1,485303:1,485304:1,485305:1,485306:1,485308:1,485309:1,485310:1,485311:1,485312:1,485332:1,485318:1,485316:1,485317:1,485319:1,485321:1,485322:1,485323:1,485327:1,485328:1,485330:1,485329:1,485334:1,485335:1,485337:1,485340:1,485341:1,485343:1,485344:1,485345:1,485346:1,485347:1,485348:1,485349:1,485350:1,485351:1,485352:1,485353:1,485355:1,485356:1,485358:1,485359:1,485360:1,485361:1,485362:1,485363:1,485365:1,485366:1,485367:1,485368:1,485369:1,485370:1,485371:1,485372:1,485373:1,485374:1,485375:1,485376:1,485377:1,485378:1,485379:1,485380:1,485381:1,485382:1,485384:1,485385:1,485386:1,485388:1,485389:1,485390:1,485391:1,485392:1,485393:1,485394:1,485395:1,485396:1,485397:1,485398:1,485399:1,485400:1,485402:1,485403:1,485404:1,485405:1,485406:1,485407:1,485408:1,485409:1,485410:1,485411:1,485412:1,485413:1,485414:1,485415:1,485416:1,485417:1,485418:1,485419:1,485420:1,485421:1,485422:1,485423:1,485424:1,485425:1,485426:1,485427:1,485428:1,485429:1,485430:1,485431:1,485432:1,485433:1,485434:1,485435:1,485436:1,485437:1,485438:1,485439:1,485440:1,485442:1,485443:1};

try {
if (acc_list[_ap_an.site] == 1)
{
    _ap_an.bigsea_rd();
}
} catch (e) {}

/* ======================== publisher ======================== */

create_ifrtrk();

function wr_aff_pub(n, l, h) {
  if (typeof l == 'undefined') l=1; if (typeof h == 'undefined') h=1;
  document.write('<img src="http://astatic.weborama.fr/transp.gif" width='+l+' height='+h+'/>');
}

var _ap_ad       = new Object;
var wr_secure    = '';
var wr_solutions = 'solution.weborama.fr/fcgi-bin/diff.fcgi?';

_ap_ad.write = function(src) {
    document.write('<scr'+'ipt type="text/'+'jav'+'ascri'+'pt" src="'+src+'"></script>');
}
_ap_ad.host        = ( _ap_defined('wr_host') )        ? wr_host : 'aimfar';
_ap_ad.board       = ( _ap_defined('wr_board') )       ? 'ide='+wr_board : null;
_ap_ad.boardsize   = ( _ap_defined('wr_boardsize') )   ? 'emp='+wr_boardsize : 'emp=1x1';
_ap_ad.autopage    = ( _ap_defined('wr_detail') )      ? 'kp='+wr_detail : null;
_ap_ad.floating    = ( _ap_defined('wr_floating') )    ? 'floating='+wr_floating : 'floating=0';
_ap_ad.site        = ( _ap_defined('wr_site') )        ? 'ids='+wr_site : null;
_ap_ad.page        = ( _ap_defined('wr_page') )        ? 'pageid='+wr_page : null;
_ap_ad.extparams   = ( _ap_defined('wr_extparams') )   ? 'ext_params='+escape(wr_extparams) : null;
_ap_ad.crealist    = ( _ap_defined('wr_crealist') )    ? 'cl='+wr_crealist : null;
_ap_ad.advlist     = ( _ap_defined('wr_advlist') )     ? 'al='+wr_advlist: null;
_ap_ad.camplist    = ( _ap_defined('wr_camplist') )    ? 'cal='+wr_camplist : null;
_ap_ad.model       = ( _ap_defined('wr_model') )       ? 'model='+wr_model : null;
_ap_ad.thema       = ( _ap_defined('wr_thema') )       ? 'wthema='+wr_thema : null;
_ap_ad.click       = ( _ap_defined('wr_click') )       ? 'ext_click='+escape(wr_click) : null;
_ap_ad.ex_secteurs = ( _ap_defined('wr_ex_secteurs' )) ? 'nsa='+escape(wr_ex_secteurs) : null;
_ap_ad.backup 	   = ( _ap_defined('wr_backup' )) 	   ? 'bak='+escape(wr_backup) : null;
_ap_ad.cache 	     = ( _ap_defined('wr_cache' )) 	     ? 'cache='+wr_cache : null;
_ap_ad.ref		     = null;
_ap_ad.url		     = null;
try {
	_ap_ad.url='url='+escape(document.location);
	var ref = (top!=null && top.location!=null && typeof(top.location.href)=="string")?top.document.referrer:document.referrer;
	_ap_ad.ref='ref='+escape(ref);	
} catch (e) {}

_ap_adlist       = [ 'site', 'page', 'boardsize', 'autopage', 'board', 'crealist',
                     'advlist', 'camplist', 'model', 'thema', 'floating', 'extparams', 'ref', 'url', 'click',
                     'ex_secteurs', 'backup', 'cache'
                   ];
var _ap_script   = 'http' + wr_secure + '://' + _ap_ad.host + '.' + wr_solutions;
var _ap_first    = true;


for ( var k in _ap_adlist ) {
    var key = _ap_adlist[k];
    if ( 'undefined' != typeof(_ap_ad[key]) && _ap_ad[key] != null ) {
        _ap_script += (_ap_first ? '' : '&') + _ap_ad[key];
        _ap_first = false;
    }
}
if (_ap_ad.board != null) _ap_ad.write(_ap_script);

/* ======================== advertiser ======================== */
