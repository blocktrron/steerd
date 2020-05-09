include $(TOPDIR)/rules.mk

PKG_NAME:=steerd
PKG_SOURCE_DATE:=2020-05-09
PKG_RELEASE:=1

PKG_SOURCE_PROTO:=git
PKG_SOURCE_URL:=https://github.com/blocktrron/steerd.git
PKG_SOURCE_VERSION:=c9650424a6d137fd927a542e4a55a1c2b887bfea

PKG_MAINTAINER:=David Bauer <mail@david-bauer.net>
PKG_LICENSE:=GPL-2.0-only
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_PARALLEL:=1

include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/cmake.mk

define Package/steerd
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Daemon for band-steering WiFi clients
  URL:=https://github.com/blocktrron/steerd.git
  DEPENDS:=$(DRV_DEPENDS) +libubus +libubox +libblobmsg-json +libuci +libiwinfo
endef

define Package/steerd/description
  Daemon for band-steering WiFi clients.
endef

define Package/steerd/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(INSTALL_BIN) $(PKG_INSTALL_DIR)/usr/sbin/steerd $(1)/usr/sbin/steerd
endef

$(eval $(call BuildPackage,steerd))
