function Component()
{
    // 默认构造函数
}

Component.prototype.createOperations = function()
{
    // 调用基类函数
    component.createOperations();

    if (systemInfo.productType === "windows") {
        // 创建开始菜单快捷方式
        component.addOperation("CreateShortcut", 
                              "@TargetDir@/student_management.exe", 
                              "@StartMenuDir@/学生管理系统.lnk",
                              "workingDirectory=@TargetDir@");

        // 创建桌面快捷方式
        component.addOperation("CreateShortcut", 
                              "@TargetDir@/student_management.exe", 
                              "@DesktopDir@/学生管理系统.lnk",
                              "workingDirectory=@TargetDir@");
    }
}
