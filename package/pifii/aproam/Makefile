include $(TOPDIR)/rules.mk

PKG_NAME:=aproam
PKG_RELEASE:=1.1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/kernel.mk

define Package/aproam
  SECTION:=pifii
  CATEGORY:=PiFii
  TITLE:=AR95xx AP Roam
  DEPENDS:=+lua
endef

define Package/aproam/description
  An aproam program.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Configure
endef

define Package/aproam/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/getterminal $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aproamrequest.lua $(1)/usr/sbin/aproamrequest
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aproamresponse.lua $(1)/usr/sbin/aproamresponse
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/aproam.init $(1)/etc/init.d/aproam
endef


$(eval $(call BuildPackage,aproam))

