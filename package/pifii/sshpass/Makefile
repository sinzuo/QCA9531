include $(TOPDIR)/rules.mk

PKG_NAME:=sshpass
PKG_VERSION:=1.05
PKG_RELEASE:=1

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@GNU/$(PKG_NAME)
PKG_MD5SUM:=c52d65fdee0712af6f77eb2b60974ac7
PKG_MAINTAINER:=<iweb.dl.sourceforge.net>

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/sshpass
  SECTION:=pifii
  CATEGORY:=PiFii
  TITLE:=SSH Remote Login
  URL:=http://iweb.dl.sourceforge.net/project/sshpass/sshpass/1.05/sshpass-1.05.tar.gz
endef

define Package/sshpass/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/sshpass $(1)/usr/bin/sshpass
endef
$(eval $(call BuildPackage,sshpass))
