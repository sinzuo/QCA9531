#
# Copyright (C) 2006,2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ap-router
PKG_VERSION:=1.2.1
PKG_RELEASE=1




include $(INCLUDE_DIR)/package.mk

define Package/ap-router
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=ap-router select 
endef


define Build/Compile
endef




define Package/ap-router/install
	$(INSTALL_DIR) $(1)/sbin
	$(CP)	./files/sbin/* $(1)/sbin/
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP)	./files/usr/lib/* $(1)/usr/lib/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) ./files/etc/config/sets $(1)/etc/config/sets
endef


$(eval $(call BuildPackage,ap-router))
