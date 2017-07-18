#
# Copyright (C) 2006,2015 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=crontabs-reboot
PKG_VERSION:=1.2.1
PKG_RELEASE=1




include $(INCLUDE_DIR)/package.mk

define Package/crontabs-reboot
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=crontabs-reboot select 
endef


define Build/Compile
endef




define Package/crontabs-reboot/install
	$(INSTALL_DIR) $(1)/etc
	$(CP)	./files/crontabs $(1)/etc/
endef


$(eval $(call BuildPackage,crontabs-reboot))
