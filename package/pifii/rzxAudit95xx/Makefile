include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=rzxAudit95xx
PKG_RELEASE:=1.0.7

#PKG_DIR:=$(shell pwd)
#PKG_BUILD_DIR:=$(PKG_DIR)/src
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rzxAudit95xx
        SECTION:=pifii
        CATEGORY:=PiFii
        TITLE:=RenZixing Audit for QCA95xx device
        DEPENDS:=+lua +luci-lib-json +luasocket +libstdcpp +apmodedhcplist
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/rzxAudit95xx/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/lsap $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rzxUserStatus.lua $(1)/usr/sbin/rzxUserStatus
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rzxUserController.sh $(1)/usr/sbin/rzxUserController
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) $(PKG_BUILD_DIR)/rzx.conf $(1)/etc/config/rzx
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rzxUserStatus.init $(1)/etc/init.d/rzxUserStatus
endef
$(eval $(call BuildPackage,rzxAudit95xx))
