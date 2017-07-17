#
# Copyright (C) 2006,2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=facttory
PKG_VERSION:=1.2.1
PKG_RELEASE=1




include $(INCLUDE_DIR)/package.mk

define Package/facttory
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=facttory select 
endef


define Build/Compile
endef




define Package/facttory/install
	$(INSTALL_DIR) $(1)/sbin
	$(CP)	./files/sbin/* $(1)/sbin/
endef


$(eval $(call BuildPackage,facttory))
