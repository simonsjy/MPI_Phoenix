//在ID为“js_menu”的div内开始程序
function initMenu()
{
      var pm_menu = new JSMenu("js_menu");
      pm_menu.init();
}
//定义主函数
function JSMenu(id) 
{
    if (!document.getElementById || !document.getElementsByTagName)
        return false;
    this.menu = document.getElementById(id);
    this.submenus = this.menu.getElementsByTagName("div");
}
//引入函数，取得所有span
JSMenu.prototype.init = function() 
{
    var mainInstance = this;
    for (var i = 0; i < this.submenus.length; i++)
        this.submenus[i].getElementsByTagName("span")[0].onclick = function() 
        {
            mainInstance.toggleMenu(this.parentNode);
        };
    this.expandOne();
};
//展开含"current"的菜单
JSMenu.prototype.expandOne = function() 
{
    for (var i = 0; i < this.submenus.length; i++) 
	{
        var links = this.submenus[i].getElementsByTagName("a");
        for (var j = 0; j < links.length; j++)
		{
            if (links[j].className == "current")
            this.expandMenu(this.submenus[i]);
        }
    }
};
//变换菜单状态函数
JSMenu.prototype.toggleMenu = function(submenu) 
{
    if (submenu.className == "collapsed")
        this.expandMenu(submenu);
    else
        this.collapseMenu(submenu);
};
//展开所有菜单函数
JSMenu.prototype.expandMenu = function(submenu) 
{
    var fullHeight = submenu.getElementsByTagName("span")[0].offsetHeight;
    var links = submenu.getElementsByTagName("a");
    for (var i = 0; i < links.length; i++)
        fullHeight += links[i].offsetHeight;
    var moveBy = Math.round(5 * links.length);
    
    var mainInstance = this;
    var intId = setInterval(function() 
	{
        var curHeight = submenu.offsetHeight;
        var newHeight = curHeight + moveBy;
        if (newHeight < fullHeight)
            submenu.style.height = newHeight + "px";
        else {
            clearInterval(intId);
            submenu.style.height = "";
            submenu.className = "";
        }
    }, 30);
    this.collapseOthers(submenu);
};
//折叠菜单函数
JSMenu.prototype.collapseMenu = function(submenu) {
    var minHeight = submenu.getElementsByTagName("span")[0].offsetHeight;
    var moveBy = Math.round(5 * submenu.getElementsByTagName("a").length);
    var mainInstance = this;
    var intId = setInterval(function() {
        var curHeight = submenu.offsetHeight;
        var newHeight = curHeight - moveBy;
        if (newHeight > minHeight)
            submenu.style.height = newHeight + "px";
        else {
            clearInterval(intId);
            submenu.style.height = "";
            submenu.className = "collapsed";
        }
    }, 30);
};
//折叠其他菜单函数
JSMenu.prototype.collapseOthers = function(submenu) {
        for (var i = 0; i < this.submenus.length; i++)
            if (this.submenus[i] != submenu && this.submenus[i].className != "collapsed")
                this.collapseMenu(this.submenus[i]);
};