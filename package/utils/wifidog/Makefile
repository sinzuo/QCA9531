#
# Copyright (C) 2006,2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wifidog
PKG_VERSION:=1.2.1
PKG_RELEASE=1




include $(INCLUDE_DIR)/package.mk

define Package/wifidog
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+iptables-mod-extra +iptables-mod-ipopt +iptables-mod-nat-extra +libpthread +libiwinfo +libevent2 +libevent2-openssl libevent2-pthreads 
  TITLE:=wifidog select 
endef


define Build/Compile
endef




define Package/wifidog/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) ./files/usr/bin/wifidog $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)	./files/usr/lib/* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) ./files/wifidog.key $(1)/etc/
	$(INSTALL_DATA) ./files/wifidog.crt $(1)/etc/
	$(INSTALL_DATA) ./files/wifidog.ca $(1)/etc/
	$(INSTALL_DATA) ./files/wifidog-msg.html $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/wifidog $(1)/etc/config/wifidog
	$(INSTALL_DIR) $(1)/sbin
	$(CP)	./files/sbin/* $(1)/sbin/
	$(INSTALL_DIR) $(1)/bin
	$(CP)	./files/bin/* $(1)/bin/
endef


$(eval $(call BuildPackage,wifidog))
