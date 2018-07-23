include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=localReportServer95xx
PKG_RELEASE:=2.0.0

#PKG_DIR:=$(shell pwd)
#PKG_BUILD_DIR:=$(PKG_DIR)/src
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/localReportServer95xx
        SECTION:=pifii
        CATEGORY:=PiFii
        TITLE:=AR95xx AP report to local AC 
        DEPENDS:=+lua +luasec +curl +luci-lib-json +luasocket
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/localReportServer95xx/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/localreportserver.lua $(1)/usr/sbin/localreportserver
	$(INSTALL_DIR) $(1)/usr/lib/lua
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/pifiilocalreport.lua $(1)/usr/lib/lua/
	$(INSTALL_DIR) $(1)/usr/lib/lua/ssl
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/pifiihttps.lua $(1)/usr/lib/lua/ssl/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/localreport.conf $(1)/etc/config/localreport
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/localreportserver.init $(1)/etc/init.d/localreportserver
endef
$(eval $(call BuildPackage,localReportServer95xx))
