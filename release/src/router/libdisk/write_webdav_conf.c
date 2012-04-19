/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <bcmnvram.h>
#include <shutils.h>

#include "usb_info.h"
#include "disk_initial.h"
#include "disk_share.h"

#define WEBDAV_CONF "/tmp/lighttpd.conf"

#define PRODUCTID "productid"
#define WEBDAV_HTTP_PORT "webdav_http_port"
#define WEBDAV_HTTPS_PORT "webdav_https_port"
char* get_productid()
{
   nvram_get(PRODUCTID);
}

char* get_webdav_http_port()
{
   nvram_get(WEBDAV_HTTP_PORT);
}

char* get_webdav_https_port()
{
   nvram_get(WEBDAV_HTTPS_PORT);
}

int main(int argc, char *argv[]) {
	FILE *fp;
	int n=0, sh_num=0;
	disk_info_t *follow_disk, *disks_info = NULL;
	partition_info_t *follow_partition;
	
	/* 
		st_webdav_mpde = 1 => share mode (default)
		st_webdav_mode = 2 => account mode
	*/

	/* write /tmp/lighttpd.conf */
	if ((fp=fopen(WEBDAV_CONF, "r"))) {
		fclose(fp);
		unlink(WEBDAV_CONF);
	}
	
	fp = fopen(WEBDAV_CONF, "w");
	if (fp==NULL) return;
	
	/* Load modules */
	fprintf(fp, "server.modules+=(\"mod_alias\")\n");
	fprintf(fp, "server.modules+=(\"mod_userdir\")\n");
	fprintf(fp, "server.modules+=(\"mod_smbdav_access\")\n");
	fprintf(fp, "server.modules+=(\"mod_webdav\")\n");
	fprintf(fp, "server.modules+=(\"mod_smbdav\")\n");
	fprintf(fp, "server.modules+=(\"mod_redirect\")\n");
	fprintf(fp, "server.modules+=(\"mod_compress\")\n");
	fprintf(fp, "server.modules+=(\"mod_usertrack\")\n");
	fprintf(fp, "server.modules+=(\"mod_rewrite\")\n");
	
	if (nvram_match("st_webdav_mode", "2")){
		fprintf(fp, "server.modules+=(\"mod_access\")\n");
		fprintf(fp, "server.modules+=(\"mod_auth\")\n");
	}

	/* Basic setting */
	fprintf(fp, "server.port=%s\n",get_webdav_http_port()); // defult setting, but no use
//	fprintf(fp, "server.port=8999\n"); // defult setting, but no use
//	fprintf(fp, "server.document-root=\"/mnt/\"\n");
	fprintf(fp, "server.document-root=\"/tmp/lighttpd/www\"\n");
	fprintf(fp, "server.upload-dirs=(\"/tmp/lighttpd/uploads\")\n");
	fprintf(fp, "server.errorlog=\"/tmp/lighttpd/err.log\"\n");
	fprintf(fp, "server.pid-file=\"/tmp/lighttpd/lighttpd.pid\"\n");
	fprintf(fp, "server.arpping-interface=\"br0\"\n");
	fprintf(fp, "dir-listing.activate=\"enable\"\n");

	//	**** Minetype setting **** //	
	fprintf(fp, "mimetype.assign = (\n");
	fprintf(fp, "\".html\" => \"text/html\",\n");
	fprintf(fp, "\".htm\" => \"text/html\",\n");
	fprintf(fp, "\".css\" => \"text/css\",\n");
	fprintf(fp, "\".js\" => \"text/javascript\",\n");
	fprintf(fp, "\".txt\" => \"text/plain\",\n");
	fprintf(fp, "\".jpg\" => \"image/jpeg\",\n");
	fprintf(fp, "\".gif\" => \"image/gif\",\n");
	fprintf(fp, "\".png\" => \"image/png\",\n");
	fprintf(fp, "\".pdf\" => \"application/pdf\",\n");
	fprintf(fp, "\".mp4\" => \"video/mp4\",\n");
	fprintf(fp, "\".mp3\" => \"audio/mpeg\"");
	fprintf(fp, ")\n");

	// **** Index file names **** //
	fprintf(fp, "index-file.names = ( \"index.php\", \"index.html\",\n");
	fprintf(fp, "\"index.htm\", \"default.htm\",\n");
	fprintf(fp, " \" index.lighttpd.html\" )\n");

	// **** url access deny
	fprintf(fp, " url.access-deny             = ( \"~\", \".inc\" )\n");

	// **** static-file.exclude extensions
	fprintf(fp," static-file.exclude-extensions = ( \".php\", \".pl\", \".fcgi\" )\n");

	// ****
	fprintf(fp, "compress.cache-dir          = \"/tmp/lighttpd/compress/\"\n");
	fprintf(fp, "compress.filetype           = ( \"application/x-javascript\", \"text/css\", \"text/html\", \"text/plain\" )\n");
	
	// **** SambaDav setting
	fprintf(fp, "$SERVER[\"socket\"]==\":%s\"{\n", get_webdav_http_port());
//	fprintf(fp, "$SERVER[\"socket\"]==\":8999\"{\n");
//	fprintf(fp, "	alias.url=(\"/webdav\"=>\"/mnt/\")\n");
//	fprintf(fp, "   $HTTP[\"url\"]=~\"^/usbdisk($|/)\"{\n");
	fprintf(fp, "   $HTTP[\"url\"]=~\"^/%s($|/)\"{\n",get_productid());
    fprintf(fp, "       server.document-root = \"/\"\n");
//	fprintf(fp, "       alias.url=(\"/usbdisk\"=>\"/mnt\")\n");
	fprintf(fp, "       alias.url=(\"/%s\"=>\"/mnt\")\n", get_productid());
    fprintf(fp, "       webdav.activate=\"enable\"\n");
    fprintf(fp, "       webdav.is-readonly=\"disable\"\n");
    fprintf(fp, "       webdav.sqlite-db-name=\"/tmp/lighttpd/webdav.db\"\n");
    fprintf(fp, "   }\n");
	fprintf(fp, "	else $HTTP[\"url\"]=~\"^/smb($|/)\"{\n");
	fprintf(fp, "		server.document-root = \"/\"\n");
	fprintf(fp, "		alias.url=(\"/smb\"=>\"/usr\")\n");
	fprintf(fp, "		smbdav.auth_ntlm = (\"Microsoft-WebDAV\",\"xxBitKinex\",\"WebDrive\")\n");
	fprintf(fp, "		webdav.activate=\"enable\"\n");
	fprintf(fp, "		webdav.is-readonly=\"disable\"\n");
//	fprintf(fp, "		webdav.sqlite-db-name=\"/tmp/lighttpd/webdav.db\"\n");
	fprintf(fp, "	}\n");
	fprintf(fp, "	else $HTTP[\"url\"] =~ \"^/favicon.ico$\"{\n");
    fprintf(fp, "		server.document-root = \"/\"\n");
    fprintf(fp, "		alias.url = ( \"/favicon.ico\" => \"/usr/css/favicon.ico\" ) \n"); 
    fprintf(fp, "		webdav.activate = \"enable\" \n");
    fprintf(fp, "		webdav.is-readonly = \"enable\"\n");
	fprintf(fp, "	}\n");
	fprintf(fp, "	else $HTTP[\"url\"] !~ \"^/smb($|/)\" { \n");
	fprintf(fp, "	    server.document-root = \"smb://\" \n");
   	fprintf(fp, "	    smbdav.activate = \"enable\" \n");
    fprintf(fp, "	    smbdav.is-readonly = \"disable\" \n");
    fprintf(fp, "	    smbdav.always-auth = \"enable\" \n");
    fprintf(fp, "	    smbdav.sqlite-db-name = \"/tmp/lighttpd/smbdav.db\" \n");
    fprintf(fp, "	    usertrack.cookie-name = \"SMBSESSID\" \n");
	fprintf(fp, "	}\n");
	fprintf(fp, "}\n");



#if 0
	/*** Webdav_setting ***/
	/* default : http://192.168.1.1:8082/webdav */
	fprintf(fp, "$SERVER[\"socket\"]==\":8082\"{\n");
	fprintf(fp, "	alias.url=(\"/webdav\"=>\"/mnt/\")\n"); 	//mount point
	fprintf(fp, "	$HTTP[\"url\"]=~\"^/webdav($|/)\"{\n");
	fprintf(fp, "		webdav.activate=\"enable\"\n");
	fprintf(fp, "		webdav.is-readonly=\"disable\"\n");
	fprintf(fp, "		webdav.sqlite-db-name=\"/tmp/lighttpd/webdav.db\"\n");
	fprintf(fp, "	}\n");

	fprintf(fp, "	auth.backend=\"plain\"\n");
	fprintf(fp, "	auth.backend.plain.userfile=\"/tmp/lighttpd/permissions\"\n");
	fprintf(fp, "	auth.require= (\"\"=>(\"method\"=>\"basic\",\n");
	fprintf(fp, "				\"realm\"=>\"webdav\",\n");
	fprintf(fp, "				\"require\"=>\"valid-user\"))\n");

	
	disks_info = read_disk_data();
	if (disks_info == NULL) {
		//printf("[webdav] fail in read disk data!!\n");
		usb_dbg("Couldn't get disk list when writing smb.conf!\n");
	//	goto confpage;
		goto WEBDAV_SETTING;
	}

	/* account permissions */
	if (nvram_match("st_webdav_mode", "2")){

		disks_info = read_disk_data();
		if (disks_info == NULL) {
			usb_dbg("Couldn't get disk list when writing lighttpd.conf!\n");
			//	goto confpage;
			goto WEBDAV_SETTING;
		}

		int acc_num;
		char **account_list;
	

		/* folder permissions */
		if (get_account_list(&acc_num, &account_list) < 0) {
			usb_dbg("Can't read the account list.\n");
			//printf("[webdav] fail in get account list\n");
			free_2_dimension_list(&acc_num, &account_list);
//			goto confpage;
			goto WEBDAV_SETTING;
		}

		for (follow_disk = disks_info; follow_disk != NULL; follow_disk = follow_disk->next) {
			for (follow_partition = follow_disk->partitions; follow_partition != NULL; follow_partition = follow_partition->next) {
				if (follow_partition->mount_point == NULL)
					continue;
				
				char **folder_list;
				int i;
				char tmp1[32];
				char *tmp2, *tmp_pre, *tmp_aft;

				strcpy(tmp1, follow_partition->mount_point);

				tmp2 = tmp1;
				for (i=0; i<3; i++){
					tmp_pre = strsep(&tmp2, "/");
					tmp_aft = tmp2;
				}
				//printf("[webdav] tmp1=%s,tmp2=%s,tmp_pre=%s,tmp_aft=%s\n", tmp1, tmp2, tmp_pre, tmp_aft);
				
				// 1. get the folder list
				if (get_folder_list(follow_partition->mount_point, &sh_num, &folder_list) < 0) {
					//printf("[webdav] fail in get folder list\n");
					usb_dbg("Can't read the folder list in %s.\n", follow_partition->mount_point);
					free_2_dimension_list(&sh_num, &folder_list);
					continue;
				}
	
				/* 
					right = 0 => NO
					right = 1 => R
					right = 3 => R/W (default)
				*/
	
				// 2. start to get every share
				for (n = 0; n < sh_num; ++n) {
					int i, right;
					for (i = 0; i < acc_num; ++i) {
						right = get_permission(account_list[i], follow_partition->mount_point, folder_list[n], "webdav");
	//printf("[webdav] (%d,%d) : right=%d, account=%s, folder=%s, mount=%s, tmp_aft=%s\n", n, i, right, account_list[i], folder_list[n], follow_partition->mount_point, tmp_aft);
						if(right == 0){
							/* forbid to access the folder */
							fprintf(fp, "	$HTTP[\"url\"]=~\"^/webdav/%s/%s($|/)\"{\n", tmp_aft, folder_list[n]);
							fprintf(fp, "		url.access-deny=(\"\")\n"); 
							fprintf(fp, "	}\n");
						}
						else if (right == 1){
							fprintf(fp, "	$HTTP[\"url\"]=~\"^/webdav/%s/%s($|/)\"{\n", tmp_aft, folder_list[n]);
							fprintf(fp, "		webdav.is-readonly=\"enable\"\n");
							fprintf(fp, "	}\n");
						}
					}
				}
				free_2_dimension_list(&sh_num, &folder_list);
			}
		}
		/* folder permissions */
		free_2_dimension_list(&acc_num, &account_list);
	}/* account permissions */
WEBDAV_SETTING:
	fprintf(fp, "}\n"); /*** Webdav_setting ***/
#endif
	/*** Webdav_SSL ***/
	/* default : https://192.168.1.1:443/webdav */
	fprintf(fp, "$SERVER[\"socket\"]==\":%s\"{\n",get_webdav_https_port());
	fprintf(fp, "	ssl.pemfile=\"/tmp/lighttpd/server.pem\"\n");
	fprintf(fp, "	ssl.engine=\"enable\"\n");
	//fprintf(fp, "	alias.url=(\"/webdav\"=>\"/mnt/\")\n"); 
//	fprintf(fp, "	$HTTP[\"url\"]=~\"^/usbdisk($|/)\"{\n");
	fprintf(fp, "	$HTTP[\"url\"]=~\"^/%s($|/)\"{\n", get_productid());
    fprintf(fp, "       server.document-root = \"/\"\n");
//	fprintf(fp, "       alias.url=(\"/usbdisk\"=>\"/mnt\")\n");	
	fprintf(fp, "       alias.url=(\"/%s\"=>\"/mnt\")\n", get_productid());	
	fprintf(fp, "		webdav.activate=\"enable\"\n");
	fprintf(fp, "		webdav.is-readonly=\"disable\"\n");
	fprintf(fp, "		webdav.sqlite-db-name=\"/tmp/lighttpd/webdav.db\"\n");
	fprintf(fp, "	}\n");
	fprintf(fp, "	else $HTTP[\"url\"]=~\"^/smb($|/)\"{\n");
	fprintf(fp, "		server.document-root = \"/\"\n");
	fprintf(fp, "		alias.url=(\"/smb\"=>\"/usr\")\n");
	fprintf(fp, "		smbdav.auth_ntlm = (\"Microsoft-WebDAV\",\"xxBitKinex\",\"WebDrive\")\n");
	fprintf(fp, "		webdav.activate=\"enable\"\n");
	fprintf(fp, "		webdav.is-readonly=\"disable\"\n");
//	fprintf(fp, "		webdav.sqlite-db-name=\"/tmp/lighttpd/webdav.db\"\n");
	fprintf(fp, "	}\n");
	fprintf(fp, "	else $HTTP[\"url\"] =~ \"^/favicon.ico$\"{\n");
    fprintf(fp, "		server.document-root = \"/\"\n");
    fprintf(fp, "		alias.url = ( \"/favicon.ico\" => \"/usr/css/favicon.ico\" ) \n"); 
    fprintf(fp, "		webdav.activate = \"enable\" \n");
    fprintf(fp, "		webdav.is-readonly = \"enable\"\n");
	fprintf(fp, "	}\n");
	fprintf(fp, "	else $HTTP[\"url\"] !~ \"^/smb($|/)\" { \n");
	fprintf(fp, "	    server.document-root = \"smb://\" \n");
   	fprintf(fp, "	    smbdav.activate = \"enable\" \n");
    fprintf(fp, "	    smbdav.is-readonly = \"disable\" \n");
    fprintf(fp, "	    smbdav.always-auth = \"enable\" \n");
    fprintf(fp, "	    smbdav.sqlite-db-name = \"/tmp/lighttpd/smbdav.db\" \n");
    fprintf(fp, "	    usertrack.cookie-name = \"SMBSESSID\" \n");
	fprintf(fp, "	}\n");
#if 0
	/* account permissions */
	if (nvram_match("st_webdav_mode", "2")){
			fprintf(stderr,"=================================================>st mod =2");

		disks_info = read_disk_data();
		if (disks_info == NULL) {
			usb_dbg("Couldn't get disk list when writing lighttpd.conf!\n");
			fprintf(stderr, "=========================================================>error1");
			goto confpage;
		}

		int acc_num;
		char **account_list;
	
		fprintf(fp, "	auth.backend=\"plain\"\n");
		fprintf(fp, "	auth.backend.plain.userfile=\"/tmp/lighttpd/permissions\"\n");
		fprintf(fp, "	auth.require= (\"\"=>(\"method\"=>\"basic\",\n");
		fprintf(fp, "				\"realm\"=>\"webdav\",\n");
		fprintf(fp, "				\"require\"=>\"valid-user\"))\n");

		/* folder permissions */
		if (get_account_list(&acc_num, &account_list) < 0) {
			//usb_dbg("Can't read the account list.\n");
			//printf("[webdav] fail in get account list\n");
			free_2_dimension_list(&acc_num, &account_list);
			fprintf(stderr, "================================================================>error2");
			goto confpage;
		}

		for (follow_disk = disks_info; follow_disk != NULL; follow_disk = follow_disk->next) {
			for (follow_partition = follow_disk->partitions; follow_partition != NULL; follow_partition = follow_partition->next) {
				if (follow_partition->mount_point == NULL)
					continue;
				
				char **folder_list;
				int i;
				//charles: dont init the size of tmp1, the name of share foler may exceed the buffer.
//				char tmp1[32];
				char *tmp1;
				char *tmp2, *tmp_pre, *tmp_aft;
				
				int tmp1_size=0;
				tmp1_size = strlen(follow_partition->mount_point)+1;
				tmp1 = malloc(tmp1_size); memset(tmp1, 0, tmp1_size);
				strcpy(tmp1, follow_partition->mount_point);

				tmp2 = tmp1;
				for (i=0; i<3; i++){
					tmp_pre = strsep(&tmp2, "/");
					tmp_aft = tmp2;
				}
				
				// 1. get the folder list
				if (get_folder_list(follow_partition->mount_point, &sh_num, &folder_list) < 0) {
					//printf("[webdav] fail in get folder list\n");
					usb_dbg("Can't read the folder list in %s.\n", follow_partition->mount_point);
					free_2_dimension_list(&sh_num, &folder_list);
					if(tmp1) free(tmp1);
					fprintf(stderr,"====================================================================>get folder list error");
					continue;
				}
	
				// 2. start to get every share
				for (n = 0; n < sh_num; ++n) {
					int i, right;
					for (i = 0; i < acc_num; ++i) {
						right = get_permission(account_list[i], follow_partition->mount_point, folder_list[n], "webdav");
	fprintf(stderr,"[webdav] (%d,%d) : right=%d, account=%s, folder=%s, mount=%s, tmp_aft=%s\n", n, i, right, account_list[i], folder_list[n], follow_partition->mount_point, tmp_aft);
						if(right == 0){
							/* forbid to access the folder */
							fprintf(fp, "	$HTTP[\"url\"]=~\"^/webdav/%s/%s($|/)\"{\n", tmp_aft, folder_list[n]);
							fprintf(fp, "		url.access-deny=(\"\")\n"); 
							fprintf(fp, "	}\n");
						}
						else if (right == 1){
						fprintf(fp, "	$HTTP[\"url\"]=~\"^/webdav/%s/%s($|/)\"{\n", tmp_aft, folder_list[n]);
							fprintf(fp, "		webdav.is-readonly=\"enable\"\n");
							fprintf(fp, "	}\n");
						}
					}
				}
				free_2_dimension_list(&sh_num, &folder_list);
				if(tmp1) free(tmp1);
			}
		}
		/* folder permissions */
		free_2_dimension_list(&acc_num, &account_list);
	}/* account permissions */
#endif
	
	goto confpage;

confpage:
	fprintf(fp, "}\n"); /*** Webdav_SSL ***/

	/* debugging */
	fprintf(fp, "debug.log-request-header=\"enable\"\n");
	fprintf(fp, "debug.log-response-header=\"enable\"\n");
	fprintf(fp, "debug.log-request-handling=\"enable\"\n");
	fprintf(fp, "debug.log-file-not-found=\"enable\"\n");
	fprintf(fp, "debug.log-condition-handling=\"enable\"\n");

	fclose(fp);
	free_disk_data(&disks_info);
	return 0;
}