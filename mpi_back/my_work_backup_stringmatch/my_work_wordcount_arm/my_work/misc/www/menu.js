//��IDΪ��js_menu����div�ڿ�ʼ����
function initMenu()
{
      var pm_menu = new JSMenu("js_menu");
      pm_menu.init();
}
//����������
function JSMenu(id) 
{
    if (!document.getElementById || !document.getElementsByTagName)
        return false;
    this.menu = document.getElementById(id);
    this.submenus = this.menu.getElementsByTagName("div");
}
//���뺯����ȡ������span
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
//չ����"current"�Ĳ˵�
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
//�任�˵�״̬����
JSMenu.prototype.toggleMenu = function(submenu) 
{
    if (submenu.className == "collapsed")
        this.expandMenu(submenu);
    else
        this.collapseMenu(submenu);
};
//չ�����в˵�����
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
//�۵��˵�����
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
//�۵������˵�����
JSMenu.prototype.collapseOthers = function(submenu) {
        for (var i = 0; i < this.submenus.length; i++)
            if (this.submenus[i] != submenu && this.submenus[i].className != "collapsed")
                this.collapseMenu(this.submenus[i]);
};