#inherit autotools pkgconfig qprebuilt
#inherit qprebuilt
#inherit systemd
#inherit update-rc.d

LICENSE = "CLOSED"
PR = "r0"

FILESEXTRAPATHS_prepend := "${THISDIR}/files/src:"
#FILES_${PN} += "/lib/systemd/*"
#
#SYSTEMD_AUTO_ENABLE = "enable"
#SYSTEMD_SERVICE_${PN} = "xs_cm.service"

S = "${WORKDIR}/src"
SRC_URI += "file://src"
SRC_URI += "file://start_xs_trigger"

CFLAGS += "-I${STAGING_KERNEL_BUILDDIR}/usr/include \
          "

CFLAGS += "-DTARGET_4G01"

#LDFLAGS += "-L${STAGING_LIBDIR} \
#            -lpthread \
#           "
LDFLAGS += "-L${STAGING_LIBDIR} \
           "

# Runlevels for data daemons in mdm targets
pkg_postinst_${PN}_mdm () {
	[ -n "$D" ] && OPT="-r $D" || OPT="-s"

	update-rc.d $OPT -f start_xs_trigger remove
	update-rc.d $OPT start_xs_trigger start 99 S . stop 20 0 1 6 .
}


do_compile() {
	oe_runmake
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 xs_trigger ${D}${bindir}

	install -d ${D}${sysconfdir}/config
	install -m 0755 ${THISDIR}/files/xs_trigger.conf ${D}${sysconfdir}/config/xs_trigger.conf
	
	install -d ${D}/etc/reset2default
	install -m 0755 ${THISDIR}/files/record_current_time.sh ${D}/etc/reset2default/record_current_time.sh
	install -m 0755 ${THISDIR}/files/determine_if_reset.sh  ${D}/etc/reset2default/determine_if_reset.sh
	install -m 0755 ${THISDIR}/files/flash_led.sh           ${D}/etc/reset2default/flash_led.sh
		
	install -m 0755 ${THISDIR}/files/start_xs_trigger -D ${D}${sysconfdir}/init.d/start_xs_trigger
}

#INHIBIT_PACKAGE_DEBUG_SPLIT = "1"
