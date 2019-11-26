ADD_CUSTOM_COMMAND(
    OUTPUT .security-manager-setup
    COMMAND sed '/--DB\.SQL--/r db.sql' security-manager-setup > .security-manager-setup
    DEPENDS security-manager-setup db.sql
)

# Add a dummy build target to trigger building of ${TARGET_DB}
ADD_CUSTOM_TARGET(DB ALL DEPENDS .security-manager-setup)

INSTALL(PROGRAMS .security-manager-setup DESTINATION ${BIN_INSTALL_DIR})
