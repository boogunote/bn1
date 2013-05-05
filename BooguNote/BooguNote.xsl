<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" exclude-result-prefixes="xsl">
    <xsl:output method="html" encoding="utf-8" indent="no"
    doctype-public="-//W3C//DTD XHTML 1.0 Transitional//EN"
    doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"/>
    <xsl:template match="/">
    <html>
    <head>
    <style type="text/css"><![CDATA[body,div,ul,li,pre,p{margin:0;padding:0}q:before,q:after{content:''}q{line-height:1.2;vertical-align:middle}body{font-size:10pt;font-family:Microsoft YaHei,SimSun,Arial Narrow,arial,serif}a{color:#0072E6;font-style:italic;text-decoration:none}a:hover,a:active{text-decoration:underline}noscript{color:#f00;font-size:2em}.t1{font-size:1em}.t2{font-size:1.3em}.t3{font-size:1.8em}.img-s img{max-height:75px}.img-m img{max-height:150px}.img-b img{max-height:300px}#notice{border:1px solid #5692D4;margin:10px;padding:20px 5px;color:#FF0000;text-indent:1.5em}#notice acronym{color:#000}#tree{margin-top:20px}#tree,#tree ul{padding-left:1.5em;list-style:none}#tree li{margin:1px 0}#tree div{border:1px solid #fff;padding:1px 0;position:relative;min-height:1.4em;height:auto;*height:1.4em}#tree div:hover,#tree  div.hover{border-color:#5692D4}#tree q{width:13px;border:0;display:block;background-color:#fff;background-repeat:no-repeat;background-position:center center;background-image:url("data:image/gif;base64,R0lGODlhCQAJAIAAAFaS1P///yH5BAAAAAAALAAAAAAJAAkAAAIPhI+hG8brXgNy1vgwQxwUADs=");position:absolute;left:-1.25em;height:13px}#tree q.open{background-image:url("data:image/gif;base64,R0lGODlhCQAJAIAAAFaS1P///yH5BAAAAAAALAAAAAAJAAkAAAIQhI+hG8brXgPzTHllfKiDAgA7");cursor:pointer}#tree q.close{background-image:url("data:image/gif;base64,R0lGODlhCQAJAIAAAFaS1P///yH5BAAAAAAALAAAAAAJAAkAAAIRhI+hG7bwoJINIktzjizeUwAAOw==");cursor:pointer}#tree div.icon{margin-left:1em}#tree div.icon q{left:-2.25em;*left:-2em}#tree label{width:16px;background-repeat:no-repeat;background-image:url("data:image/gif;base64,R0lGODlhEABwAMQAAA+ECQoWsfr26vTelDw8QfpIJdimk+uZWHt9g9IPLdN/UO/QK5HJB32W87rnt6waJv3puszKy9j3/et7LKTE9c3Y/ClRsvHj58NFQfrJ3O/gVvq7tVWjFYW9b9N7h////yH5BAAAAAAALAAAAAAQAHAAAAX/4CeOZClaWHR9kYd5kXhJ51MoLnZgmIE9DxGFUJhMMIXCQZF8xE7II1IJfAqJxWnBaUIlf0CP6dPAhq1dQnVMalgQFTapYrHIS27KfUS370UIFnp/FG9/hyIOHRyMHB0OY4scDJQMjB00I4uVnJYdmpOdlQ6OH6SilQIQloqhoh8ClI6MqAICm4wAjbsCH7u/khy9jbrEALAAvR0AzM2Ozb2mzdOPHMyfDtPOkMvM2drXmuDUJYvgpZG7n4hjCGhyEQQE7zIGIxEI8vYjF/EI9/jkIeh1IR+CFTJEGBAY8CCbgAIRPpRHQOKYCxTlycFIwJ3BixEvFKxYYuQ8GRj//5GI19EiRov5SJZwt9KdRX7s9iigN8ZAhn17DFwwwJPEUAEDitYboMpnCQUGoho4MOACBKlYp2oYwLWrVw07Pww4sGCrhrNoFyhYqWCB27cLJgDl13bC2wMqS5LVUJavGBMGzOrYeqDn2BQGFHA1MUDBgb8sMCyYKzaFCQ9rR9haTALCAAjRSEig0KB0AwqZRBeqw7pOhdRkWDeoQJo1aiGsJUh4rTv3Bwl1AqBe3WB0gAAWRls4fnp5Hd3IhTdA7vx4AN3TmWe3flyAhOnBS3M/Hme79vEBXqOfjT74ePXvK6DP9P7CeH7y0VuPU2L5eD9skDZbTn9c4IEHN5kwFN0QQt1xAQYJJIBBgpptAAQQG4RW0g8RPjDhGAJY+EAGGWCooQweACFSGDcJcMGFK6qooQApJjBiiTZ6oOGLD0i4wQYQqqiZBxHaeGGROoogQI9Fklikjb3Q+GQCTj6p44NTZinhBRtoqeUGnW2QmGOfnchCVJ81NsEBoJEggAEbQCDnAEZABUGFBsi5wQFG8HkAmEoa8NmeRhS6ZjQCKCCnAoYWqgCiitHZaJ2IJjZpoQeEBgGjlyrWmQGXrnlnZ3w2yuYYECxR56kgyulqmyCOqQBUZnbm6h9LQGVCCAA7");font-weight:bold;text-align:center;text-indent:-99em;position:absolute;left:-17px;height:16px;overflow:hidden;*text-indent:0}#tree label.flag{color:red}#tree label.tick{color:#39C10F;background-position:0 -16px}#tree label.cross{color:#403840;background-position:0 -32px}#tree label.star{color:#FFC140;background-position:0 -48px}#tree label.question{color:#00f;background-position:0 -64px}#tree label.warning{color:red;background-position:0 -80px}#tree label.idea{color:#DD9500;background-position:0 -96px}#tree *q{width:9px;border:1px solid #5692D4;left:-1em;top:3px;height:9px;overflow:hidden}#tree *q.open{background-color:#80FFFF}#tree *q.close{background-color:#FF8080}img{border:0;width:auto;height:auto}#tree pre{white-space:pre-wrap;*white-space:pre;*word-wrap:break-word}#panel{border:1px solid #5692D4;padding:2px 18px 2px 10px;display:none;color:#fff;background-color:#5692D4;font-size:9pt;position:fixed;right:3%;top:0;box-shadow:1px 1px 8px #444;border-radius:5px;*position:absolute}#panel:hover{border-color:#ffff00}#panel em a{padding:0 0.5em;margin:0;color:#fff;font-weight:bold;text-shadow:1px 1px 8px #000}#panel span{display:block;color:#375D81;font-size:8pt;position:relative}#panel span a{margin:0 0.5em 0 0;padding:1px 0.5em;border:1px solid;border-color:#6FB3D4 #3D6794 #3D6794 #6FB3D4;display:inline-block;color:#FFFF80;background-color:#5692D4;font-style:normal;font-size:9pt}#panel span a:hover{color:#5DFF43;text-decoration:none}#options{border:1px solid #93B4D9;display:none;color:#000;background-color:#CADEF4;font-size:9pt;position:fixed;right:3%;top:50px;border-radius:5px;box-shadow:2px 2px 4px #000;*position:absolute}#options fieldset{float:left;margin:20px 10px;border:1px solid #5679A5}#options legend{font-weight:bold;text-shadow:1px 1px 3px #888}#options label{display:block}.btn-close{border:1px solid #5692D4;width:1em;color:#fff;background-color:#5679A5;text-align:center;line-height:0.8em;position:absolute;right:2px;top:2px;height:1em;cursor:pointer;border-radius:999px}.btn-close:hover{color:#f00;background-color:#FFFF80}]]></style>
    </head>
    <body id="body">
        <noscript>Javascript is disabled. Please enable it for advance features.<br />您的浏览器禁用了javascript, 将不能使用高级功能.</noscript>
        <xsl:comment>
            <![CDATA[[if IE 6.0]>
            <div id="notice">
                <p>This browser is too old. For best viewing experience, please upgrade you IE to latest version, or choose <a href="http://www.google.com/chrome">Chrome</a>, <a href="http://www.mozilla.com">Firefox</a>, <a href="http://www.opera.com">Opera</a>. Thanks.</p>
                <p>我们发现您正在使用<strong>旧版本</strong>的<acronym title="Internet Explorer">IE</acronym>浏览器。</p>
                <p>为了给您提供更好的阅读体验，请尽快升级您的浏览器，或者使用<a href="http://www.google.com/chrome">Chrome</a>、<a href="http://www.mozilla.com">Firefox</a>、<a href="http://www.opera.com">Opera</a>等现代浏览器。</p></div>
            <![endif]]]>
        </xsl:comment>
        <div id="main">
            <xsl:apply-templates/>
        </div>
        <div id="panel"></div>
        <div id="options"></div>
        <script type="text/javascript"><![CDATA[var e=false,h=!-[1,]&&!window.XMLHttpRequest;(function(b){function c(d,g){for(var f=/\{%([_a-z]+)%\}/i,l;l=d.match(f);)d=d.replace(RegExp(l[0],"g"),g[l[1]]||"");return d}switch(b.toLowerCase()){case "zh-cn":b={btn_title:"\u4fee\u6539\u56fe\u7247\u548c\u6587\u5b57\u5927\u5c0f",btn:"\u8bbe\u7f6e",author:"\u6a21\u677f\u5236\u4f5c",generate:"\u8be5HTML\u7531",by:"\u751f\u6210",url:"/index_zh.htm",utl_title:"\u4e2d\u6587\u4e3b\u9875",close:"\u5173\u95ed",image:"\u56fe\u7247\u5927\u5c0f",text:"\u6587\u5b57\u5927\u5c0f",small:"\u5c0f",medium:"\u4e2d",big:"\u5927",original:"\u539f\u59cb\u5927\u5c0f"};break;default:b={btn_title:"Change image size and text size",btn:"Style",author:"XSL Author",generate:"Generated by",utl_title:"Home page",close:"Close",image:"Image size",text:"Text size",small:"small",medium:"medium",big:"big",original:"original"}}var a=i("panel");a.innerHTML=c('<p><span><a id="btn-option" href="#" title="{%btn_title%}">{%btn%}</a>{%author%}:dishuostec</span>{%generate%}<em><a href="http://boogu.me{%url%}" title="BooguNote {%utl_title%}">BooguNote</a></em>{%by%}</p><div id="panel-close" class="btn-close" title="{%close%}">x</div>',b);a.style.display="block";i("options").innerHTML=c('<fieldSet id="option-img"><legend>{%image%}</legend><label><input type="radio" name="img" value="img-s" />{%small%}</label><label><input type="radio" name="img" value="img-m" checked="checked" />{%medium%}</label><label><input type="radio" name="img" value="img-b" />{%big%}</label><label><input type="radio" name="img" value="img-a" />{%original%}</label></fieldSet><fieldSet id="option-txt"><legend>{%text%}</legend><label><input type="radio" name="txt" value="t1" checked="checked" />{%small%}</label><label><input type="radio" name="txt" value="t2" />{%medium%}</label><label><input type="radio" name="txt" value="t3" />{%big%}</label></fieldSet><fieldSet id="option-tree"><legend>\u6298\u53e0</legend><label><input type="button" name="tr1" value="\u5168\u6298\u53e0" /></label><label><input type="button" name="tr2" value="\u5168\u5c55\u5f00" /></label></fieldSet><div id="option-close" class="btn-close" title="{%close%}">x</div>',b)})(window.navigator.userLanguage||window.navigator.language||"");var k=function(){function b(c){return function(a){if(c.apply(this,arguments)===e){if(a&&a.stopPropagation)a.stopPropagation();else window.event.cancelBubble=true;if(a&&a.preventDefault)a.preventDefault();else window.event.returnValue=e}}}return document.attachEvent?function(c,a,d){if(!c)return e;c["e"+a+d]=b(d);c[a+d]=function(){c["e"+a+d](window.event)};c.attachEvent("on"+a,c[a+d])}:function(c,a,d){if(!c)return e;c.addEventListener(a,b(d),e)}}();function i(b){return document.getElementById(b)}function m(b,c){return(c||document).getElementsByTagName(b)}function n(){this.className="open";this.parentNode.nextSibling.style.display="block"}function o(){this.className="close";this.parentNode.nextSibling.style.display="none"}(function(b){for(var c=b.length-1;c>=0;c--)b[c].className!="none"&&k(b[c],"click",function(a){return function(){(a=!a)?o.call(this):n.call(this)}}(b[c].className=="close"))})(m("q",i("tree")));(function(b){function c(f,l,p){var j=f.href;if(l.test(j)){j="file:///"+j.replace(p,"/");f.href=j}if(f.className=="file"){j=f.innerHTML.split("/");f.innerHTML=j[j.length-1]}else(f=m("img",f)[0])&&(f.src=j)}for(var a=/^[a-z]:/i,d=/\\/g,g=b.length-1;g>=0;g--)c(b[g],a,d)})(m("a",i("tree")));(function(b){function c(d){var g=d.innerHTML,f=/\bhttps?:\/\/[\-A-Z0-9+&@#\/%?=~_|$!:,.;]*[A-Z0-9+&@#\/%=~_|$]/ig;if(f.test(g))d.innerHTML=g.replace(f,'<a href="$&">$&</a>')}for(var a=b.length-1;a>=0;a--)c(m("span",b[a])[0])})(m("pre"),i("tree"));k(i("btn-option"),"click",function(){i("options").style.display="block";return e});k(i("option-close"),"click",function(){i("options").style.display="none";return e});k(i("panel-close"),"click",function(){i("panel").style.display="none";return e});k(i("options"),"click",function(b){var c=b.target||b.srcElement;if(c.tagName.toLowerCase()==="input")switch(c.parentNode.parentNode.id){case "option-img":if(h){switch(c.value){case "img-s":b=75;break;case "img-m":b=150;break;case "img-b":b=300;break;default:b=0}c=m("img");for(var a=c.length-1;a>=0;a--)c[a].b(b)}else i("tree").className=c.value;break;case "option-txt":i("main").className=c.value;break;case "option-tree":b=m("q",i("tree"));c=c.name=="tr1"?o:n;for(a=b.length-1;a>=0;a--)b[a].className!="none"&&c.call(b[a])}});(function(b){function c(g){var f=this.a;this.style.height=g>0&&g<f?g:f}if(!b)return e;var a,d;b=m("img",i("tree"));for(a=b.length-1;a>=0;a--){d=b[a];d.a=d.height;d.b=c}b=m("div",i("tree"));for(a=b.length-1;a>=0;a--){k(b[a],"mouseover",function(){this.className+=" hover"});k(b[a],"mouseout",function(){this.className=this.className.replace(" hover","")})}})(h);
]]></script>
    </body>
    </html>
    </xsl:template>

    <!--树-->
    <xsl:template match="root">
        <ul id="tree" class="img-m">
            <xsl:apply-templates select="item"/>
        </ul>
    </xsl:template>

    <!--生成节点-->
    <xsl:template match="item">
        <xsl:variable name="Count" select="count(item)" />
        <xsl:variable name="branch" select="@branch" />
        <li>
            <div>
                <!--小图标-->
                <xsl:if test="@icon != 'none'">
                    <xsl:attribute name="class">icon</xsl:attribute>
                    <xsl:call-template name="Node-icon">
                        <xsl:with-param name="icon" select="@icon" />
                    </xsl:call-template>
                </xsl:if>
                <!--折叠控制框-->
                <q>
                    <xsl:attribute name="class"><xsl:value-of select="$branch" /></xsl:attribute>
                    <xsl:if test="$branch = 'none'"><xsl:attribute name="disabled">disabled</xsl:attribute></xsl:if>
                </q>
                <!--内容-->
                <xsl:call-template name="Node-text">
                    <xsl:with-param name="content" select="@content" />
                </xsl:call-template>
            </div>
            <!--下级节点-->
            <xsl:if test="$Count &gt; 0">
                <ul>
                    <xsl:if test="$branch = 'close'">
                        <xsl:attribute name="style">display:none;</xsl:attribute>
                    </xsl:if>
                    <xsl:apply-templates select="item"/>
                </ul>
            </xsl:if>
        </li>
    </xsl:template>

    <!--内容-->
    <xsl:template name="Node-text">
        <xsl:param name="content" />
        <xsl:variable name="TextColor" select="substring(@TextColor,1,6)" />
        <xsl:variable name="BkgrdColor" select="substring(@BkgrdColor,1,6)" />
        <xsl:variable name="IsBold" select="@IsBold" />
        <xsl:variable name="IsFile" select="@IsFile = 'true'" />
        <xsl:variable name="IsImage" select="@IsImage = 'true'" />
        <xsl:choose>
            <xsl:when test="$IsFile">
                <xsl:variable name="FileUrl" select="translate($content,'\','/')" />
                <span>
                    <!--样式-->
                    <xsl:attribute name="style">
                        <xsl:if test="$TextColor != '000000'">color:#<xsl:value-of select="$TextColor" />;</xsl:if>
                        <xsl:if test="$BkgrdColor != 'ffffff'">background-color:#<xsl:value-of select="$BkgrdColor" />;</xsl:if>
                        <xsl:if test="$IsBold = 'true'">font-weight:bold;</xsl:if>
                    </xsl:attribute>

                    <xsl:element name="a">
                        <xsl:attribute name="href">
                            <xsl:value-of select="$FileUrl" />
                        </xsl:attribute>
                        <xsl:if test="not($IsImage)">
                            <xsl:attribute name="class">file</xsl:attribute>
                            <xsl:value-of select="$FileUrl" />
                        </xsl:if>
                        <xsl:if test="$IsImage">
                            <xsl:attribute name="class">image</xsl:attribute>
                            <xsl:element name="img">
                                <xsl:attribute name="src">
                                    <xsl:value-of select="$FileUrl" />
                                </xsl:attribute>
                            </xsl:element>
                        </xsl:if>
                    </xsl:element>
                </span>
            </xsl:when>
            <xsl:otherwise>
                <pre>
                    <span>
                        <!--样式-->
                        <xsl:attribute name="style">
                            <xsl:if test="$TextColor != '000000'">color:#<xsl:value-of select="$TextColor" />;</xsl:if>
                            <xsl:if test="$BkgrdColor != 'ffffff'">background-color:#<xsl:value-of select="$BkgrdColor" />;</xsl:if>
                            <xsl:if test="$IsBold = 'true'">font-weight:bold;</xsl:if>
                        </xsl:attribute>
                        <xsl:value-of select="$content"/>
                    </span>
                </pre>
            </xsl:otherwise>
        </xsl:choose>
    </xsl:template>

    <!--生成标签-->
    <xsl:template name="Node-icon">
        <xsl:param name="icon" />
        <label>
            <xsl:attribute name="class">
                <xsl:value-of select="$icon" />
            </xsl:attribute>
            <xsl:choose>
                <xsl:when test="$icon = 'flag'">※</xsl:when>
                <xsl:when test="$icon = 'tick'">√</xsl:when>
                <xsl:when test="$icon = 'cross'">×</xsl:when>
                <xsl:when test="$icon = 'star'">★</xsl:when>
                <xsl:when test="$icon = 'question'">?</xsl:when>
                <xsl:when test="$icon = 'warning'">!</xsl:when>
                <xsl:when test="$icon = 'idea'">⊙</xsl:when>
            </xsl:choose>
        </label>
    </xsl:template>
</xsl:stylesheet>