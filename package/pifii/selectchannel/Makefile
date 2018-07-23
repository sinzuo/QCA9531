include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=selectchannel
PKG_RELEASE:=1.0.2

#PKG_DIR:=$(shell pwd)
#PKG_BUILD_DIR:=$(PKG_DIR)/src
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/selectchannel
        SECTION:=pifii
        CATEGORY:=PiFii
        TITLE:=Select a good channel for wireless (Lua Script)
        DEPENDS:=+lua 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/selectchannel/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/selectchannel $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/selectchannel.init $(1)/etc/init.d/selectchannel
endef
$(eval $(call BuildPackage,selectchannel))
