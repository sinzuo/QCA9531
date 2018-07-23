include $(TOPDIR)/rules.mk
# Name and release number of this package
PKG_NAME:=rzxInstallGram95xx
PKG_RELEASE:=V1.0.0.21_062003_Release_20180530

#PKG_DIR:=$(shell pwd)
#PKG_BUILD_DIR:=$(PKG_DIR)/src
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/rzxInstallGram95xx
        SECTION:=pifii
        CATEGORY:=PiFii
        TITLE:=RenZixing Audit InstallGam for QCA95xx 
        DEPENDS:=+rzxAudit95xx +libstdcpp +zlib
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./files/* $(PKG_BUILD_DIR)/
endef

define Package/rzxInstallGram95xx/install
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(CP) $(PKG_BUILD_DIR)/Install_gram $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/rzxInstallGram95xx.init $(1)/etc/init.d/rzxInstallGram95xx
endef
$(eval $(call BuildPackage,rzxInstallGram95xx))
