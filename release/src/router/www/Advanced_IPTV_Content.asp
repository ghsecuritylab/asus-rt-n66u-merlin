<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<html xmlns:v>
<head>
<meta http-equiv="X-UA-Compatible" content="IE=EmulateIE7"/>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta HTTP-EQUIV="Pragma" CONTENT="no-cache">
<meta HTTP-EQUIV="Expires" CONTENT="-1">
<link rel="shortcut icon" href="images/favicon.png">
<link rel="icon" href="images/favicon.png">
<title>ASUS Wireless Router <#Web_Title#> - IPTV</title>
<link rel="stylesheet" type="text/css" href="index_style.css"> 
<link rel="stylesheet" type="text/css" href="form_style.css">
<link rel="stylesheet" type="text/css" href="other.css">
<script type="text/javascript" src="/state.js"></script>
<script type="text/javascript" src="/general.js"></script>
<script type="text/javascript" src="/popup.js"></script>
<script type="text/javascript" src="/help.js"></script>
<script type="text/javascript" src="/detect.js"></script>

<script>
<% login_state_hook(); %>
var wireless = [<% wl_auth_list(); %>];	// [[MAC, associated, authorized], ...]

function initial(){
	show_menu();
	disable_udpxy();
	
	if(Rawifi_support == -1)	//rawifi platform without this item, by Viz 2012.01
		$('enable_eff_multicast_forward').style.display="";
		
}

function applyRule(){
	if(document.form.udpxy_enable_x.value != 0 && document.form.udpxy_enable_x.value != ""){
		if(!validate_range(document.form.udpxy_enable_x, 1024, 65535)){
				document.form.udpxy_enable_x.focus();
				document.form.udpxy_enable_x.select();
				return false;
		}else{ 
			showLoading();
			document.form.submit();
		}
	}else{
		showLoading();
		document.form.submit();		
	}	
	
}

function valid_udpxy(){
	if(document.form.udpxy_enable_x.value != 0 && document.form.udpxy_enable_x.value != ""){
		if(!validate_range(document.form.udpxy_enable_x, 1024, 65535)){
				document.form.udpxy_enable_x.focus();
				document.form.udpxy_enable_x.select();
				return false;
		}else 
			return true;
	}	
}

function disable_udpxy(){
	if(document.form.mr_enable_x.value == 1){
		return change_common_radio(document.form.mr_enable_x, 'RouterConfig', 'mr_enable_x', '1');
	}
	else{	
		return change_common_radio(document.form.mr_enable_x, 'RouterConfig', 'mr_enable_x', '0');
	}	
}// The input fieldof UDP proxy does not relate to Mutlicast Routing. 

function add_options_value(o, str, arr, orig){
        if(orig == arr)
                add_option(o, str, arr, 1);
        else
                add_option(o, str, arr, 0);
}
</script>
</head>

<body onload="initial();" onunLoad="return unload_body();">
<div id="TopBanner"></div>
<div id="hiddenMask" class="popup_bg">
	<table cellpadding="5" cellspacing="0" id="dr_sweet_advise" class="dr_sweet_advise" align="center">
		<tr>
		<td>
			<div class="drword" id="drword" style="height:110px;"><#Main_alert_proceeding_desc4#> <#Main_alert_proceeding_desc1#>...
				<br/>
				<br/>
	    </div>
		  <div class="drImg"><img src="images/alertImg.png"></div>
			<div style="height:70px;"></div>
		</td>
		</tr>
	</table>
<!--[if lte IE 6.5]><iframe class="hackiframe"></iframe><![endif]-->
</div>

<div id="Loading" class="popup_bg"></div>

<iframe name="hidden_frame" id="hidden_frame" src="" width="0" height="0" frameborder="0"></iframe>

<form method="post" name="form" id="ruleForm" action="/start_apply.htm" target="hidden_frame">
<input type="hidden" name="productid" value="<% nvram_get("productid"); %>">
<input type="hidden" name="current_page" value="Advanced_IPTV_Content.asp">
<input type="hidden" name="next_page" value="">
<input type="hidden" name="next_host" value="">
<input type="hidden" name="group_id" value="">
<input type="hidden" name="modified" value="0">
<input type="hidden" name="action_mode" value="apply">
<input type="hidden" name="action_script" value="restart_net">
<input type="hidden" name="action_wait" value="10">
<input type="hidden" name="preferred_lang" id="preferred_lang" value="<% nvram_get("preferred_lang"); %>">
<input type="hidden" name="firmver" value="<% nvram_get("firmver"); %>">

<table class="content" align="center" cellpadding="0" cellspacing="0">
  <tr>
	<td width="17">&nbsp;</td>
	
	<!--=====Beginning of Main Menu=====-->
	<td valign="top" width="202">
	  <div id="mainMenu"></div>
	  <div id="subMenu"></div>
	</td>
	
    <td valign="top">
	<div id="tabMenu" class="submenuBlock"></div>
		<!--===================================Beginning of Main Content===========================================-->
<table width="98%" border="0" align="left" cellpadding="0" cellspacing="0">
	<tr>
		<td align="left" valign="top">
  <table width="760px" border="0" cellpadding="5" cellspacing="0" class="FormTitle" id="FormTitle">
	<tbody>
	<tr>
		  <td bgcolor="#4D595D" valign="top"  >
		  <div>&nbsp;</div>
		  <div class="formfonttitle"><#menu5_2#> - IPTV</div>
      <div style="margin-left:5px;margin-top:10px;margin-bottom:10px"><img src="/images/New_ui/export/line_export.png"></div>
      <div class="formfontdesc"><#LANHostConfig_displayIPTV_sectiondesc#></div>
		  
		  <table width="100%" border="1" align="center" cellpadding="4" cellspacing="0" bordercolor="#6b8fa3"  class="FormTable">

			<!-- 2008.03 James. patch for Oleg's patch. { -->		  
			<!--tr>
				<th><#RouterConfig_GWDHCPEnable_itemname#></th>
				<td>
					<input type="radio" value="1" name="dr_enable_x" class="input" onClick="return change_common_radio(this, 'RouterConfig', 'dr_enable_x', '1')" <% nvram_match("dr_enable_x", "1", "checked"); %>><#checkbox_Yes#>
					<input type="radio" value="0" name="dr_enable_x" class="input" onClick="return change_common_radio(this, 'RouterConfig', 'dr_enable_x', '0')" <% nvram_match("dr_enable_x", "0", "checked"); %>><#checkbox_No#>
				</td>
			</tr-->
			<tr>
				<th><#RouterConfig_GWDHCPEnable_itemname#></th>
				<td>
          <select name="dr_enable_x" class="input_option">
          	<option value="0" <% nvram_match("dr_enable_x", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
            <option value="1" <% nvram_match("dr_enable_x", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
          </select>
				</td>
			</tr>

			<!--tr>
				<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,11);"><#RouterConfig_GWMulticastEnable_itemname#></a></th>
				<td>
					<input type="radio" value="1" name="mr_enable_x" class="input" onClick="disable_udpxy();" <% nvram_match("mr_enable_x", "1", "checked"); %>><#checkbox_Yes#>
					<input type="radio" value="0" name="mr_enable_x" class="input" onClick="disable_udpxy();" <% nvram_match("mr_enable_x", "0", "checked"); %>><#checkbox_No#>
				</td>
			</tr-->	
			<tr>
				<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(5,11);"><#RouterConfig_GWMulticastEnable_itemname#></a></th>
				<td>
          <select name="mr_enable_x" class="input_option">
            <option value="0" <% nvram_match("mr_enable_x", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
           	<option value="1" <% nvram_match("mr_enable_x", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
          </select>
				</td>
			</tr>

					<tr id="enable_eff_multicast_forward" style="display:none;">
						<th>Enable efficient multicast forwarding</th>
						<td>
                  				<select name="emf_enable" class="input_option">
                    					<option value="0" <% nvram_match("emf_enable", "0","selected"); %> ><#WLANConfig11b_WirelessCtrl_buttonname#></option>
                    					<option value="1" <% nvram_match("emf_enable", "1","selected"); %> ><#WLANConfig11b_WirelessCtrl_button1name#></option>
                  				</select>
						</td>
					</tr>
			<!-- 2008.03 James. patch for Oleg's patch. } -->
			<tr>
				<th><a class="hintstyle" href="javascript:void(0);" onClick="openHint(6, 6);"><#RouterConfig_IPTV_itemname#></a></th>
     		<td>
     			<input id="udpxy_enable_x" type="text" maxlength="5" class="input_6_table" name="udpxy_enable_x" value="<% nvram_get("udpxy_enable_x"); %>" onkeypress="return is_number(this,event);">
     		</td>
     	</tr>
		</table>	

		<div class="apply_gen">
			<input class="button_gen" onclick="applyRule()" type="button" value="<#CTL_apply#>"/>
		</div>
		
	  </td>
	</tr>

	</tbody>	
  </table>		
					
		</td>
	</form>					
				</tr>
			</table>				
			<!--===================================End of Main Content===========================================-->
</td>

    <td width="10" align="center" valign="top">&nbsp;</td>
	</tr>
</table>

<div id="footer"></div>
</body>
</html>