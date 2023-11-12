// TimeOut
// Copyright (c) 2023 CM0use

function Component()
{
    // default constructor
}

Component.prototype.createOperations = function()
{
    try {
        component.createOperations();
    } catch (e) {
        console.log(e);
    }

    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut",
                               "@TargetDir@/TimeOut.exe",
                               "@StartMenuDir@/TimeOut.lnk");
        component.addOperation("CreateShortcut",
                               "@TargetDir@/TimeOut.exe",
                               "@DesktopDir@/TimeOut.lnk");

        component.addOperation("CreateShortcut",
                               "@TargetDir@/maintenancetool.exe",
                               "@StartMenuDir@/Uninstall.lnk",
                               "iconPath=@TargetDir@/TimeOut.exe");
    } else if (installer.value("os") === "x11") {
        component.addOperation("InstallIcons", "@TargetDir@/icons");
        var app_desktop = "Version=1.0.0\nName=TimeOut\nComment=A timer with customized program/command executions\nComment[es]=Un temporizador con ejecuciones personalizadas de programas y comandos\nExec=@TargetDir@/TimeOut\nTerminal=false\nType=Application\nIcon=TimeOutIcon\nCategories=Utility;Security",
        uninstall_desktop = "Version=1.0.0\nName=Uninstall\nExec=@TargetDir@/maintenancetool\nTerminal=false\nKeyword=TimeOut\nType=Application\nIcon=TimeOutIcon\nCategories=Utility;Security";

        component.addOperation("CreateDesktopEntry",
                               "TimeOut.desktop",
                               app_desktop);

        component.addOperation("CreateDesktopEntry",
                               "Uninstall.desktop",
                               uninstall_desktop);
    }
}
