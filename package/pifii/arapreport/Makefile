include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=arapreport
PKG_RELEASE:=1.0.3

#PKG_DIR:=$(shell pwd)
#PKG_BUILD_DIR:=$(PKG_DIR)/src
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/arapreport
        SECTION:=pifii
        CATEGORY:=PiFii
        TITLE:=AR953X AP report to local AC --Lua or Shell Script 
        DEPENDS:=+lua +luasec +curl +luci-lib-json +luasocket
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/arapreport/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apreport $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/pifii.lua $(1)/usr/lib/lua/
	$(INSTALL_DIR) $(1)/usr/lib/lua/ssl
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/pifiihttps.lua $(1)/usr/lib/lua/ssl/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/pifii.conf $(1)/etc/config/pifii
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/apreport.init $(1)/etc/init.d/apreport
endef
$(eval $(call BuildPackage,arapreport))
