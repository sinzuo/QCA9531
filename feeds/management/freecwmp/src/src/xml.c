/*
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>
 *	Copyright (C) 2012 Jonas Gorski <jogo@openwrt.org>
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <libfreecwmp.h>
#include <microxml.h>

#include "xml.h"

#include "config.h"
#include "cwmp.h"
#include "external.h"
#include "freecwmp.h"
#include "messages.h"


static int xml_handle_set_parameter_values(mxml_node_t *body_in,
					   mxml_node_t *tree_in,
					   mxml_node_t *tree_out);

static int xml_handle_get_parameter_values(mxml_node_t *body_in,
					   mxml_node_t *tree_in,
					   mxml_node_t *tree_out);

static int xml_handle_set_parameter_attributes(mxml_node_t *body_in,
					       mxml_node_t *tree_in,
					       mxml_node_t *tree_out);

static int xml_handle_add_object(mxml_node_t *body_in,
				 mxml_node_t *tree_in,
				 mxml_node_t *tree_out);

static int xml_handle_get_rpc_methods(mxml_node_t *body_in,
				      mxml_node_t *tree_in,
				      mxml_node_t *tree_out);

static int xml_handle_download(mxml_node_t *body_in,
			       mxml_node_t *tree_in,
			       mxml_node_t *tree_out);

static int xml_handle_factory_reset(mxml_node_t *body_in,
				    mxml_node_t *tree_in,
				    mxml_node_t *tree_out);

static int xml_handle_reboot(mxml_node_t *body_in,
			     mxml_node_t *tree_in,
			     mxml_node_t *tree_out);

static int xml_create_generic_fault_message(mxml_node_t *body,
					    bool client,
					    char *code,
					    char *string);
struct rpc_method {
	const char *name;
	int (*handler)(mxml_node_t *body_in, mxml_node_t *tree_in,
			  mxml_node_t *tree_out);
};

const static char *soap_env_url = "http://schemas.xmlsoap.org/soap/envelope/";
const static char *soap_enc_url = "http://schemas.xmlsoap.org/soap/encoding/";
const static char *xsd_url = "http://www.w3.org/2001/XMLSchema";
const static char *xsi_url = "http://www.w3.org/2001/XMLSchema-instance";
const static char *cwmp_urls[] = {
		"urn:dslforum-org:cwmp-1-0",
		"urn:dslforum-org:cwmp-1-1",
		"urn:dslforum-org:cwmp-1-2",
		NULL };

static struct cwmp_namespaces
{
	char *soap_env;
	char *soap_enc;
	char *xsd;
	char *xsi;
	char *cwmp;
} ns;

const struct rpc_method rpc_methods[] = {
	{ "SetParameterValues", xml_handle_set_parameter_values },
	{ "GetParameterValues", xml_handle_get_parameter_values },
	{ "SetParameterAttributes", xml_handle_set_parameter_attributes },
	{ "AddObject", xml_handle_add_object },
	{ "GetRPCMethods", xml_handle_get_rpc_methods },
	{ "Download", xml_handle_download },
	{ "FactoryReset", xml_handle_factory_reset },
	{ "Reboot", xml_handle_reboot },
};

static int xml_recreate_namespace(mxml_node_t *tree)
{
	const char *cwmp_urn;
	char *c;
	int i;

	FREE(ns.soap_env);
	FREE(ns.soap_enc);
	FREE(ns.xsd);
	FREE(ns.xsi);
	FREE(ns.cwmp);

	c = (char *) mxmlElementGetAttrName(tree, soap_env_url);
	if (c && *(c + 5) == ':') {
		ns.soap_env = strdup((c + 6));
	} else {
		return -1;
	}

	c = (char *) mxmlElementGetAttrName(tree, soap_enc_url);
	if (c && *(c + 5) == ':') {
		ns.soap_enc = strdup((c + 6));
	} else {
		return -1;
	}

	c = (char *) mxmlElementGetAttrName(tree, xsd_url);
	if (c && *(c + 5) == ':') {
		ns.xsd = strdup((c + 6));
	} else {
		return -1;
	}

	c = (char *) mxmlElementGetAttrName(tree, xsi_url);
	if (c && *(c + 5) == ':') {
		ns.xsi = strdup((c + 6));
	} else {
		return -1;
	}

	for (i = 0; cwmp_urls[i] != NULL; i++) {
		cwmp_urn = cwmp_urls[i];
		c = (char *) mxmlElementGetAttrName(tree, cwmp_urn);
		if (c && *(c + 5) == ':') {
			ns.cwmp = strdup((c + 6));
			break;
		}
	}

	if (!ns.cwmp) return -1;

	return 0;
}

void xml_exit(void)
{
	FREE(ns.soap_env);
	FREE(ns.soap_enc);
	FREE(ns.xsd);
	FREE(ns.xsi);
	FREE(ns.cwmp);
}

static int xml_prepare_events_inform(mxml_node_t *tree)
{
	mxml_node_t *node, *b1, *b2;
	char *c;
	int n = 0;
	struct list_head *p;
	struct event *event;

	b1 = mxmlFindElement(tree, tree, "Event", NULL, NULL, MXML_DESCEND);
	if (!b1) return -1;

	list_for_each(p, &cwmp->events) {
		event = list_entry (p, struct event, list);
		node = mxmlNewElement (b1, "EventStruct");
		if (!node) goto error;

		b2 = mxmlNewElement (node, "EventCode");
		if (!b2) goto error;

		b2 = mxmlNewText(b2, 0, lfc_str_event_code(event->code, false));
		if (!b2) goto error;

		b2 = mxmlNewElement (node, "CommandKey");
		if (!b2) goto error;

		if (event->key) {
			b2 = mxmlNewText(b2, 0, event->key);
			if (!b2) goto error;
		}

		mxmlAdd(b1, MXML_ADD_AFTER, MXML_ADD_TO_PARENT, node);
		n++;
	}

	if (n) {
		if (asprintf(&c, "cwmp:EventStruct[%u]", n) == -1)
			return -1;

		mxmlElementSetAttr(b1, "soap_enc:arrayType", c);
		FREE(c);
	}

	return 0;

error:
	return -1;
}

static int xml_prepare_notifications_inform(mxml_node_t *tree)
{
	/* notifications */
	mxml_node_t *b;
	char *c;
	struct list_head *p;
	struct notification *notification;

	b = mxmlFindElement(tree, tree, "Event", NULL, NULL, MXML_DESCEND);
	if (!b) return -1;

	list_for_each(p, &cwmp->notifications) {
		notification = list_entry(p, struct notification, list);

		c = "InternetGatewayDevice.ManagementServer.ConnectionRequestURL";
		b = mxmlFindElementText(tree, tree, c, MXML_DESCEND);
		if (!b) goto error;

		b = b->parent->parent->parent;
		b = mxmlNewElement(b, "ParameterValueStruct");
		if (!b) goto error;

		b = mxmlNewElement(b, "Name");
		if (!b) goto error;

		b = mxmlNewText(b, 0, notification->parameter);
		if (!b) goto error;

		b = b->parent->parent;
		b = mxmlNewElement(b, "Value");
		if (!b) goto error;

		b = mxmlNewText(b, 0, notification->value);
		if (!b) goto error;
	}

	return 0;

error:
	return -1;
}
/*
static int xml_prepare_inform_set(mxml_node_t *tree,char *text,const char *value)
{
	mxml_node_t * b = mxmlFindElementText(tree, tree, text, MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	char *c = NULL;
	if (NULL == value || '\0' == value[0])
	{
		if (external_get_action("value", text, &c)) goto error;
	}else
	{
		c = strdup(value);
	}
	if (c) {
		b = mxmlNewText(b, 0, c);
		FREE(c);
		if (!b) goto error;
	}
	return 0;
error:
	return -1;
}
*/
static int xml_prepare_inform_set2(mxml_node_t *tree,char *text,const char *value)
{
	mxml_node_t * b = mxmlFindElementText(tree, tree, text, MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;
	char *c = NULL;
	if (NULL == value || '\0' == value[0])
	{
		if (external_get_inform(text, &c)) goto error;
	}else
	{
		c = strdup(value);
	}
	if (c) {
		b = mxmlNewText(b, 0, c);
		FREE(c);
		if (!b) goto error;
	}
	return 0;
error:
	D("xml_prepare_inform_set2 failed");
	return -1;
}


int xml_prepare_inform_message(char **msg_out)
{
	mxml_node_t *tree, *b;
	//char *c, *tmp;
	char *tmp;

	tree = mxmlLoadString(NULL, CWMP_INFORM_MESSAGE, MXML_NO_CALLBACK);
	if (!tree) goto error;

	b = mxmlFindElement(tree, tree, "RetryCount", NULL, NULL, MXML_DESCEND);
	if (!b) goto error;

	b = mxmlNewInteger(b, cwmp->retry_count);
	if (!b) goto error;
 
	b = mxmlFindElement(tree, tree, "Manufacturer", NULL, NULL, MXML_DESCEND);
	if (!b) goto error;

	b = mxmlNewText(b, 0, config->device->manufacturer);
	if (!b) goto error;

	b = mxmlFindElement(tree, tree, "OUI", NULL, NULL, MXML_DESCEND);
	if (!b) goto error;

	b = mxmlNewText(b, 0, config->device->oui);
	if (!b) goto error;

	b = mxmlFindElement(tree, tree, "ProductClass", NULL, NULL, MXML_DESCEND);
	if (!b) goto error;

	b = mxmlNewText(b, 0, config->device->product_class);
	if (!b) goto error;

	b = mxmlFindElement(tree, tree, "SerialNumber", NULL, NULL, MXML_DESCEND);
	if (!b) goto error;

	b = mxmlNewText(b, 0, config->device->serial_number);
	if (!b) goto error;

	if (xml_prepare_events_inform(tree))
		goto error;

	b = mxmlFindElement(tree, tree, "CurrentTime", NULL, NULL, MXML_DESCEND);
	if (!b) goto error;

	b = mxmlNewText(b, 0, lfc_get_current_time(config->local->date_format));
	if (!b) goto error;

	b = mxmlFindElementText(tree, tree, "InternetGatewayDevice.DeviceInfo.Manufacturer", MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	b = mxmlNewText(b, 0, config->device->manufacturer);
	if (!b) goto error;

	b = mxmlFindElementText(tree, tree, "InternetGatewayDevice.DeviceInfo.ManufacturerOUI", MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	b = mxmlNewText(b, 0, config->device->oui);
	if (!b) goto error;

	b = mxmlFindElementText(tree, tree, "InternetGatewayDevice.DeviceInfo.ProductClass", MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	b = mxmlNewText(b, 0, config->device->product_class);
	if (!b) goto error;

	b = mxmlFindElementText(tree, tree, "InternetGatewayDevice.DeviceInfo.SerialNumber", MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	b = mxmlNewText(b, 0, config->device->serial_number);
	if (!b)
		goto error;

	b = mxmlFindElementText(tree, tree, "InternetGatewayDevice.DeviceInfo.HardwareVersion", MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	b = mxmlNewText(b, 0, config->device->hardware_version);
	if (!b) goto error;

	b = mxmlFindElementText(tree, tree, "InternetGatewayDevice.DeviceInfo.SoftwareVersion", MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	b = mxmlNewText(b, 0, config->device->software_version);
	if (!b) goto error;

	/*
	b = mxmlFindElementText(tree, tree, "InternetGatewayDevice.DeviceInfo.ProvisioningCode", MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;
	tmp = "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1.ExternalIPAddress";
	b = mxmlFindElementText(tree, tree, tmp, MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	c = NULL;
	if (external_get_action("value", tmp, &c)) goto error;
	if (c) {
		b = mxmlNewText(b, 0, c);
		FREE(c);
		if (!b) goto error;
	}

	tmp = "InternetGatewayDevice.ManagementServer.ConnectionRequestURL";
	b = mxmlFindElementText(tree, tree, tmp, MXML_DESCEND);
	if (!b) goto error;

	b = b->parent->next->next;
	if (mxmlGetType(b) != MXML_ELEMENT)
		goto error;

	c = NULL;
	if (external_get_action("value", tmp, &c)) goto error;
	if (c) {
		b = mxmlNewText(b, 0, c);
		FREE(c);
		if (!b) goto error;
	}
	*/
	tmp = "InternetGatewayDevice.DeviceInfo.memory_utilization";
	if (xml_prepare_inform_set2(tree,tmp,NULL))
		goto error;
	tmp = "InternetGatewayDevice.DeviceInfo.flash_utilization";
	if (xml_prepare_inform_set2(tree,tmp,NULL))
		goto error;

	tmp = "InternetGatewayDevice.DeviceInfo.cpu_utilization";
	if (xml_prepare_inform_set2(tree,tmp,NULL))
		goto error;

        tmp = "InternetGatewayDevice.DeviceInfo.UpTime";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.LANDevice.1.Wireless.WiFiClient";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.DeviceInfo.wireless_ssid";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.DeviceInfo.wireless_enable";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.DeviceInfo.wireless_key";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.DeviceInfo.wireless_channel_status";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.DeviceInfo.portstate";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.DeviceInfo.client_speed";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
	/*
        tmp = "InternetGatewayDevice.DeviceInfo.root_password";
        if (xml_prepare_inform_set(tree,tmp,NULL))
                goto error;
	*/
        tmp = "InternetGatewayDevice.DeviceInfo.wan_current_ip_addr";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.DeviceInfo.work_mode";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
        tmp = "InternetGatewayDevice.DeviceInfo.wifidog_enable";
        if (xml_prepare_inform_set2(tree,tmp,NULL))
                goto error;
	if (xml_prepare_notifications_inform(tree))
		goto error;
	*msg_out = mxmlSaveAllocString(tree, MXML_NO_CALLBACK);

	mxmlDelete(tree);
	return 0;

error:
	mxmlDelete(tree);
	return -1;
}

int xml_parse_inform_response_message(char *msg_in)
{
	mxml_node_t *tree = NULL, *b;
	char *c;

	if (!msg_in) goto error;

	tree = mxmlLoadString(NULL, msg_in, MXML_NO_CALLBACK);
	if (!tree) goto error;

	if (asprintf(&c, "%s:%s", ns.soap_env, "Fault") == -1)
		goto error;

	b = mxmlFindElement(tree, tree, c, NULL, NULL, MXML_DESCEND);
	FREE(c);

	// TODO: ACS responded with error message, right now we are not handeling this
	if (b) goto error;

	b = mxmlFindElement(tree, tree, "MaxEnvelopes", NULL, NULL, MXML_DESCEND);
	if (!b) goto error;

	b = mxmlWalkNext(b, tree, MXML_DESCEND_FIRST);
	if (!b || !b->value.text.string)
		goto error;

	mxmlDelete(tree);
	return 0;

error:
	mxmlDelete(tree);
	return -1;
}

int xml_handle_message(char *msg_in, char **msg_out)
{
	mxml_node_t *tree_in = NULL, *tree_out = NULL, *b;
	const struct rpc_method *method;
	char *c;
	int i;

	tree_out = mxmlLoadString(NULL, CWMP_RESPONSE_MESSAGE, MXML_NO_CALLBACK);
	if (!tree_out) goto error;

	tree_in = mxmlLoadString(NULL, msg_in, MXML_NO_CALLBACK);
	if (!tree_in) goto error;

	if (xml_recreate_namespace(tree_in)) goto error;

	/* handle cwmp:ID */
	if (asprintf(&c, "%s:%s", ns.cwmp, "ID") == -1)
		goto error;

	b = mxmlFindElement(tree_in, tree_in, c, NULL, NULL, MXML_DESCEND);
	FREE(c);

	/* ACS did not send ID parameter, we are continuing without it */
	if (!b) goto find_method;

	b = mxmlWalkNext(b, tree_in, MXML_DESCEND_FIRST);
	if (!b || !b->value.text.string) goto find_method;
	c = strdup(b->value.text.string);

	b = mxmlFindElement(tree_out, tree_out, "cwmp:ID", NULL, NULL, MXML_DESCEND);
	if (!b) goto error;

	b = mxmlNewText(b, 0, c);
	FREE(c);

	if (!b) goto error;

find_method:
	if (asprintf(&c, "%s:%s", ns.soap_env, "Body") == -1)
		goto error;

	b = mxmlFindElement(tree_in, tree_in, c, NULL, NULL, MXML_DESCEND);
	FREE(c);
	if (!b) goto error;

	while (1) {
		b = mxmlWalkNext(b, tree_in, MXML_DESCEND_FIRST);
		if (!b) goto error;
		if (b->type == MXML_ELEMENT) break;
	}

	c = b->value.element.name;
	/* convert QName to localPart, check that ns is the expected one */
	if (strchr(c, ':')) {
		char *tmp = strchr(c, ':');
		size_t ns_len = tmp - c;

		if (strlen(ns.cwmp) != ns_len)
			goto error;

		if (strncmp(ns.cwmp, c, ns_len))
			goto error;

		c = tmp + 1;
	} else {
		goto error;
	}

	method = NULL;
	for (i = 0; i < ARRAY_SIZE(rpc_methods); i++) {
		if (!strcmp(c, rpc_methods[i].name)) {
			method = &rpc_methods[i];
			break;
		}
	}

	if (method) {
		if (method->handler(b, tree_in, tree_out)) goto error;
	} else {
		char *fault_message;

		b = mxmlFindElement(tree_out, tree_out, "soap_env:Body",
				    NULL, NULL, MXML_DESCEND);
		if (!b) goto error;

		if (asprintf(&fault_message, "%s not supported", c) == -1)
			goto error;

		if (xml_create_generic_fault_message(b, true, "9000", fault_message)) {
			FREE(fault_message);
			goto error;
		}

		FREE(fault_message);
	}

	*msg_out = mxmlSaveAllocString(tree_out, MXML_NO_CALLBACK);

	mxmlDelete(tree_in);
	mxmlDelete(tree_out);
	return 0;

error:
	mxmlDelete(tree_in);
	mxmlDelete(tree_out);
	return -1;
}

static int xml_create_generic_fault_message(mxml_node_t *body,
					    bool client,
					    char *code,
					    char *string)
{
	mxml_node_t *b, *t, *u;

	b = mxmlNewElement(body, "soap_env:Fault");
	if (!b) return -1;

	t = mxmlNewElement(b, "faultcode");
	if (!t) return -1;

	u = mxmlNewText(t, 0, client ? "Client" : "Server");
	if (!u) return -1;

	t = mxmlNewElement(b, "faultstring");
	if (!t) return -1;

	u = mxmlNewText(t, 0, "CWMP fault");
	if (!u) return -1;

	b = mxmlNewElement(b, "detail");
	if (!b) return -1;

	b = mxmlNewElement(b, "cwmp:Fault");
	if (!b) return -1;

	t = mxmlNewElement(b, "FaultCode");
	if (!t) return -1;

	u = mxmlNewText(t, 0, code);
	if (!u) return -1;

	t = mxmlNewElement(b, "FaultString");
	if (!b) return -1;

	u = mxmlNewText(t, 0, string);
	if (!u) return -1;

	return 0;
}

int hexchar2int(char);

/**
 * ?src??url??
 *param src char* urlencode???????
 *return null: ???src?????,?? ?????????
 */
char* urldecode(char *input){
    int len = strlen(input);
    int count = len;
    int bbbbb = 0;
    char *src=input;
    char *dst = (char *)malloc(sizeof(char) * (count+1));
    if (! dst ) // ??????
        return NULL;
    char *dst1 = dst;
   // printf("jiangyibo 3 %d\n",*src);
    //????,?????len?count???????
    int flag = 1;
    while(*src){//???????
        if ( *src == '\\'){//??????
            src++;
            len = hexchar2int(*src);
            src++;
            count = hexchar2int(*src);
            src++;
            bbbbb = hexchar2int(*src);
            if (count == -1 || len == -1){//??????????????
                flag = 0;
                break;
            }
            //printf("jiangyibo %d %d %d \n",len,count,bbbbb);
            *dst1++ =(char)( len*64+ + count*8 + bbbbb);//????????
        }else{
	   *dst1++=*src;
        }
        src++;
    }
    *dst1 = 0;//????????\0
    return dst;
}
/**
 *?hex??????????
 *return 0~15:????,-1:??c ?? hexchar
 */
int hexchar2int(char c){
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else
        return -1;
}


int xml_handle_set_parameter_values(mxml_node_t *body_in,
				    mxml_node_t *tree_in,
				    mxml_node_t *tree_out)
{
	mxml_node_t *b = body_in;
	char *parameter_name = NULL;
	char *parameter_value = NULL;

	while (b) {
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "Name")) {
			parameter_name = b->value.text.string;
		}
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "Value")) {

			char *d = urldecode(b->value.text.string);
			 int length = strlen(b->value.text.string);
		    if(d){
		        memcpy(b->value.text.string,d,length);
		        //printf("decode is: %s \r\n", d);
		        free(d);
		    }
	
			parameter_value = b->value.text.string;

			
 		        //printf("jiangyibo %s\n",parameter_value);
		}
		if (parameter_name && parameter_value) {
			if (cwmp_set_parameter_write_handler(parameter_name,
							     parameter_value))
			{
				return -1;
			}
			parameter_name = NULL;
			parameter_value = NULL;
		}
		b = mxmlWalkNext(b, body_in, MXML_DESCEND);
	}

	if (external_set_action_execute())
		return -1;

	config_load();

	b = mxmlFindElement(tree_out, tree_out, "soap_env:Body",
			    NULL, NULL, MXML_DESCEND);
	if (!b) return -1;

	b = mxmlNewElement(b, "cwmp:SetParameterValuesResponse");
	if (!b) return -1;

	b = mxmlNewElement(b, "Status");
	if (!b) return -1;

	b = mxmlNewText(b, 0, "1");
	if (!b) return -1;

	return 0;
}

int xml_handle_get_parameter_values(mxml_node_t *body_in,
				    mxml_node_t *tree_in,
				    mxml_node_t *tree_out)
{
	mxml_node_t *n, *b = body_in;
	char *parameter_name = NULL;
	char *parameter_value = NULL;
	char *c;
	int counter = 0;

	n = mxmlFindElement(tree_out, tree_out, "soap_env:Body",
			    NULL, NULL, MXML_DESCEND);
	if (!n) return -1;

	n = mxmlNewElement(n, "cwmp:GetParameterValuesResponse");
	if (!n) return -1;

	n = mxmlNewElement(n, "ParameterList");
	if (!n) return -1;

#ifdef ACS_MULTI
	mxmlElementSetAttr(n, "xsi:type", "soap_enc:Array");
#endif

	while (b) {
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "string")) {
			parameter_name = b->value.text.string;
		}

		if (parameter_name) {
			if (!config_get_cwmp(parameter_name, &parameter_value)) {
				// got the parameter value using libuci
			} else if (!external_get_action("value",
					parameter_name, &parameter_value)) {
				// got the parameter value via external script
			} else {
				// error occurred when getting parameter value
				goto out;
			}
			counter++;

			n = mxmlFindElement(tree_out, tree_out, "ParameterList", NULL, NULL, MXML_DESCEND);
			if (!n) goto out;

			n = mxmlNewElement(n, "ParameterValueStruct");
			if (!n) goto out;

			n = mxmlNewElement(n, "Name");
			if (!n) goto out;

			n = mxmlNewText(n, 0, parameter_name);
			if (!n) goto out;

			n = n->parent->parent;
			n = mxmlNewElement(n, "Value");
			if (!n) goto out;

#ifdef ACS_MULTI
			mxmlElementSetAttr(n, "xsi:type", "xsd:string");
#endif
			n = mxmlNewText(n, 0, parameter_value ? parameter_value : "");
			if (!n) goto out;

			/*
			 * three day's work to finally find memory leak if we
			 * free parameter_name;
			 * it points to: b->value.text.string
			 *
			 * also, parameter_value can be NULL so we don't do checks
			 */
			parameter_name = NULL;
		}

		FREE(parameter_value);
		b = mxmlWalkNext(b, body_in, MXML_DESCEND);
	}

#ifdef ACS_MULTI
	b = mxmlFindElement(tree_out, tree_out, "ParameterList",
			    NULL, NULL, MXML_DESCEND);
	if (!b) goto out;

	if (asprintf(&c, "cwmp:ParameterValueStruct[%d]", counter) == -1)
		goto out;

	mxmlElementSetAttr(b, "soap_enc:arrayType", c);
	FREE(c);
#endif

	FREE(parameter_value);
	return 0;

out:
	FREE(parameter_value);
	return -1;
}

static int xml_handle_set_parameter_attributes(mxml_node_t *body_in,
					       mxml_node_t *tree_in,
					       mxml_node_t *tree_out) {

	mxml_node_t *n, *b = body_in;
	char *c, *parameter_name = NULL, *parameter_notification = NULL;
	uint8_t attr_notification_update = 0;

	/* handle cwmp:SetParameterAttributes */
	if (asprintf(&c, "%s:%s", ns.cwmp, "SetParameterAttributes") == -1)
		return -1;

	n = mxmlFindElement(tree_in, tree_in, c, NULL, NULL, MXML_DESCEND);
	FREE(c);

	if (!n) return -1;
	b = n;

	while (b != NULL) {
		if (b && b->type == MXML_ELEMENT &&
		    !strcmp(b->value.element.name, "SetParameterAttributesStruct")) {
			attr_notification_update = 0;
			parameter_name = NULL;
			parameter_notification = NULL;
		}
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "Name")) {
			parameter_name = b->value.text.string;
		}
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "NotificationChange")) {
			attr_notification_update = (uint8_t) atoi(b->value.text.string);
		}
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "Notification")) {
			parameter_notification = b->value.text.string;
		}
		if (attr_notification_update && parameter_name && parameter_notification) {
			if (external_set_action_write("notification",
					parameter_name, parameter_notification))
				return -1;
			attr_notification_update = 0;
			parameter_name = NULL;
			parameter_notification = NULL;
		}
		b = mxmlWalkNext(b, n, MXML_DESCEND);
	}

	if (external_set_action_execute())
		return -1;

	b = mxmlFindElement(tree_out, tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b) return -1;

	b = mxmlNewElement(b, "cwmp:SetParameterAttributesResponse");
	if (!b) return -1;

	config_load();

	return 0;
}

static int xml_handle_add_object(mxml_node_t *body_in,
				 mxml_node_t *tree_in,
				 mxml_node_t *tree_out)
{
	mxml_node_t *b1 = body_in, *b2 = NULL;
	char *object_name = NULL;
	char *object_instance = NULL;

	while (b1) {
		if (b1 && b1->type == MXML_TEXT &&
		    b1->value.text.string &&
		    b1->parent->type == MXML_ELEMENT &&
		    !strcmp(b1->parent->value.element.name, "ObjectName")) {
			object_name = b1->value.text.string;
		}
		if (object_name) {
			break;
		}
		b1 = mxmlWalkNext(b1, body_in, MXML_DESCEND);
	}

	if (object_name == NULL) goto error;

	if (external_object("add", object_name, &object_instance)) {
		goto error;
	}

	b1 = mxmlFindElement(tree_out, tree_out, "soap_env:Body",
			    NULL, NULL, MXML_DESCEND);
	if (!b1) return -1;

	b1 = mxmlNewElement(b1, "cwmp:AddObjectResponse");
	if (!b1) return -1;

	b2 = mxmlNewElement(b1, "InstanceNumber");
	if (!b2) return -1;

	b2 = mxmlNewText(b2, 0, object_instance);
	free(object_instance);
	if (!b2) return -1;

	b2 = mxmlNewElement(b1, "Status");
	if (!b2) return -1;

	b2 = mxmlNewText(b2, 0, "0");
	if (!b2) return -1;

	return 0;

error:
	free(object_instance);

	return -1;
}

static int xml_handle_get_rpc_methods(mxml_node_t *node,
				      mxml_node_t *tree_in,
				      mxml_node_t *tree_out)
{
	mxml_node_t *b1, *b2, *method_list;
	int num_methods = 0;

	b1 = mxmlFindElement(tree_out, tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b1) return -1;

	b1 = mxmlNewElement(b1, "cwmp:GetRPCMethodsResponse");
	if (!b1) return -1;

	method_list = mxmlNewElement(b1, "MethodList");
	if (!method_list) return -1;

	b2 = mxmlNewElement(method_list, "string");
	if (!b2) return -1;

	num_methods++;
	b2 = mxmlNewText(b2, 0, "GetRPCMethods");
	if (!b2) return -1;

	b2 = mxmlNewElement(method_list, "string");
	if (!b2) return -1;

	num_methods++;
	b2 = mxmlNewText(b2, 0, "SetParameterValues");
	if (!b2) return -1;

	b2 = mxmlNewElement(method_list, "string");
	if (!b2) return -1;

	num_methods++;
	b2 = mxmlNewText(b2, 0, "GetParameterValues");
	if (!b2) return -1;

	b2 = mxmlNewElement(method_list, "string");
	if (!b2) return -1;

	num_methods++;
	b2 = mxmlNewText(b2, 0, "SetParameterAttributes");
	if (!b2) return -1;

	b2 = mxmlNewElement(method_list, "string");
	if (!b2) return -1;

	num_methods++;
	b2 = mxmlNewText(b2, 0, "AddObject");
	if (!b2) return -1;

	b2 = mxmlNewElement(method_list, "string");
	if (!b2) return -1;

	num_methods++;
	b2 = mxmlNewText(b2, 0, "Download");
	if (!b2) return -1;

	b2 = mxmlNewElement(method_list, "string");
	if (!b2) return -1;

	num_methods++;
	b2 = mxmlNewText(b2, 0, "FactoryReset");
	if (!b2) return -1;

	b2 = mxmlNewElement(method_list, "string");
	if (!b2) return -1;

	num_methods++;
	b2 = mxmlNewText(b2, 0, "Reboot");
	if (!b2) return -1;

	char *attr_value;
	if (asprintf(&attr_value, "cwmp:string[%d]", num_methods) == -1)
		return -1;

	mxmlElementSetAttr(method_list, "soap_enc:arrayType", attr_value);
	free(attr_value);

	return 0;
}

static int xml_handle_download(mxml_node_t *body_in,
			       mxml_node_t *tree_in,
			       mxml_node_t *tree_out)
{
	mxml_node_t *n, *t, *b = body_in;
	char *c, *download_url, *download_size ,*download_md5;

	if (asprintf(&c, "%s:%s", ns.cwmp, "Download") == -1)
		return -1;

	n = mxmlFindElement(tree_in, tree_in, c, NULL, NULL, MXML_DESCEND);
	FREE(c);

	if (!n) return -1;
	b = n;

	download_url = NULL;
	download_size = NULL;
	download_md5 = NULL;
	while (b != NULL) {
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "URL")) {
			download_url = b->value.text.string;
		}
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "FileSize")) {
			download_size = b->value.text.string;
		}
		if (b && b->type == MXML_TEXT &&
		    b->value.text.string &&
		    b->parent->type == MXML_ELEMENT &&
		    !strcmp(b->parent->value.element.name, "MD5")) {
			download_md5 = b->value.text.string;
		}
		b = mxmlWalkNext(b, n, MXML_DESCEND);
	}
	if (!download_url || !download_size || !download_md5)
		return -1;

	t = mxmlFindElement(tree_out, tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!t) return -1;

	t = mxmlNewElement(t, "cwmp:DownloadResponse");
	if (!t) return -1;

	b = mxmlNewElement(t, "Status");
	if (!b) return -1;

	b = mxmlNewElement(t, "StartTime");
	if (!b) return -1;

	b = mxmlNewText(b, 0, lfc_get_current_time(config->local->date_format));
	if (!b) return -1;

	b = mxmlFindElement(t, tree_out, "Status", NULL, NULL, MXML_DESCEND);
	if (!b) return -1;

	if (external_download_md5(download_url, download_size,download_md5))
		b = mxmlNewText(b, 0, "9000");
	else
		b = mxmlNewText(b, 0, "1");

	b = mxmlNewElement(t, "CompleteTime");
	if (!b) return -1;

	b = mxmlNewText(b, 0, lfc_get_current_time(config->local->date_format));
	if (!b) return -1;

	return 0;
}

static int xml_handle_factory_reset(mxml_node_t *node,
				    mxml_node_t *tree_in,
				    mxml_node_t *tree_out)
{
	mxml_node_t *b;

	b = mxmlFindElement(tree_out, tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b) return -1;

	b = mxmlNewElement(b, "cwmp:FactoryResetResponse");
	if (!b) return -1;

	if (external_simple("factory_reset"))
		return -1;

	return 0;
}

static int xml_handle_reboot(mxml_node_t *node,
			     mxml_node_t *tree_in,
			     mxml_node_t *tree_out)
{
	mxml_node_t *b;

	b = mxmlFindElement(tree_out, tree_out, "soap_env:Body", NULL, NULL, MXML_DESCEND);
	if (!b) return -1;

	b = mxmlNewElement(b, "cwmp:RebootResponse");
	if (!b) return -1;

	if (external_simple("reboot"))
		return -1;

	return 0;
}

