"function convertToId(search)\n"
"{\n"
"  var result = '';\n"
"  for (i=0;i<search.length;i++)\n"
"  {\n"
"    var c = search.charAt(i);\n"
"    var cn = c.charCodeAt(0);\n"
"    if (c.match(/[a-z0-9\\u0080-\\uFFFF]/))\n"
"    {\n"
"      result+=c;\n"
"    }\n"
"    else if (cn<16) \n"
"    {\n"
"      result+=\"_0\"+cn.toString(16);\n"
"    }\n"
"    else \n"
"    {\n"
"      result+=\"_\"+cn.toString(16);\n"
"    }\n"
"  }\n"
"  return result;\n"
"}\n"
"\n"
"function getXPos(item)\n"
"{\n"
"  var x = 0;\n"
"  if (item.offsetWidth)\n"
"  {\n"
"    while (item && item!=document.body)\n"
"    {\n"
"      x   += item.offsetLeft;\n"
"      item = item.offsetParent;\n"
"    }\n"
"  }\n"
"  return x;\n"
"}\n"
"\n"
"function getYPos(item)\n"
"{\n"
"  var y = 0;\n"
"  if (item.offsetWidth)\n"
"  {\n"
"     while (item && item!=document.body)\n"
"     {\n"
"       y   += item.offsetTop;\n"
"       item = item.offsetParent;\n"
"     }\n"
"  }\n"
"  return y;\n"
"}\n"
"\n"
"/* A class handling everything associated with the search panel.\n"
"\n"
"   Parameters:\n"
"   name - The name of the global variable that will be \n"
"          storing this instance.  Is needed to be able to set timeouts.\n"
"   resultPath - path to use for external files\n"
"*/\n"
"function SearchBox(name, resultsPath, inFrame, label)\n"
"{\n"
"  if (!name || !resultsPath) {  alert(\"Missing parameters to SearchBox.\"); }\n"
"   \n"
"  // ---------- Instance variables\n"
"  this.name                  = name;\n"
"  this.resultsPath           = resultsPath;\n"
"  this.keyTimeout            = 0;\n"
"  this.keyTimeoutLength      = 500;\n"
"  this.closeSelectionTimeout = 300;\n"
"  this.lastSearchValue       = \"\";\n"
"  this.lastResultsPage       = \"\";\n"
"  this.hideTimeout           = 0;\n"
"  this.searchIndex           = 0;\n"
"  this.searchActive          = false;\n"
"  this.insideFrame           = inFrame;\n"
"  this.searchLabel           = label;\n"
"\n"
"  // ----------- DOM Elements\n"
"\n"
"  this.DOMSearchField = function()\n"
"  {  return document.getElementById(\"MSearchField\");  }\n"
"\n"
"  this.DOMSearchSelect = function()\n"
"  {  return document.getElementById(\"MSearchSelect\");  }\n"
"\n"
"  this.DOMSearchSelectWindow = function()\n"
"  {  return document.getElementById(\"MSearchSelectWindow\");  }\n"
"\n"
"  this.DOMPopupSearchResults = function()\n"
"  {  return document.getElementById(\"MSearchResults\");  }\n"
"\n"
"  this.DOMPopupSearchResultsWindow = function()\n"
"  {  return document.getElementById(\"MSearchResultsWindow\");  }\n"
"\n"
"  this.DOMSearchClose = function()\n"
"  {  return document.getElementById(\"MSearchClose\"); }\n"
"\n"
"  this.DOMSearchBox = function()\n"
"  {  return document.getElementById(\"MSearchBox\");  }\n"
"\n"
"  // ------------ Event Handlers\n"
"\n"
"  // Called when focus is added or removed from the search field.\n"
"  this.OnSearchFieldFocus = function(isActive)\n"
"  {\n"
"    this.Activate(isActive);\n"
"  }\n"
"\n"
"  this.OnSearchSelectShow = function()\n"
"  {\n"
"    var searchSelectWindow = this.DOMSearchSelectWindow();\n"
"    var searchField        = this.DOMSearchSelect();\n"
"\n"
"    if (this.insideFrame)\n"
"    {\n"
"      var left = getXPos(searchField);\n"
"      var top  = getYPos(searchField);\n"
"      left += searchField.offsetWidth + 6;\n"
"      top += searchField.offsetHeight;\n"
"\n"
"      // show search selection popup\n"
"      searchSelectWindow.style.display='block';\n"
"      left -= searchSelectWindow.offsetWidth;\n"
"      searchSelectWindow.style.left =  left + 'px';\n"
"      searchSelectWindow.style.top  =  top  + 'px';\n"
"    }\n"
"    else\n"
"    {\n"
"      var left = getXPos(searchField);\n"
"      var top  = getYPos(searchField);\n"
"      top += searchField.offsetHeight;\n"
"\n"
"      // show search selection popup\n"
"      searchSelectWindow.style.display='block';\n"
"      searchSelectWindow.style.left =  left + 'px';\n"
"      searchSelectWindow.style.top  =  top  + 'px';\n"
"    }\n"
"\n"
"    // stop selection hide timer\n"
"    if (this.hideTimeout) \n"
"    {\n"
"      clearTimeout(this.hideTimeout);\n"
"      this.hideTimeout=0;\n"
"    }\n"
"    return false; // to avoid \"image drag\" default event\n"
"  }\n"
"\n"
"  this.OnSearchSelectHide = function()\n"
"  {\n"
"    this.hideTimeout = setTimeout(this.name +\".CloseSelectionWindow()\",\n"
"                                  this.closeSelectionTimeout);\n"
"  }\n"
"\n"
"  // Called when the content of the search field is changed.\n"
"  this.OnSearchFieldChange = function(evt)\n"
"  {\n"
"    if (this.keyTimeout) // kill running timer\n"
"    {\n"
"      clearTimeout(this.keyTimeout);\n"
"      this.keyTimeout = 0;\n"
"    }\n"
"\n"
"    var e  = (evt) ? evt : window.event; // for IE\n"
"    if (e.keyCode==40 || e.keyCode==13)\n"
"    {\n"
"      if (e.shiftKey==1)\n"
"      {\n"
"        this.OnSearchSelectShow();\n"
"        var win=this.DOMSearchSelectWindow(); \n"
"        for (i=0;i<win.childNodes.length;i++)\n"
"        {\n"
"          var child = win.childNodes[i]; // get span within a\n"
"          if (child.className=='SelectItem')\n"
"          {\n"
"            child.focus();\n"
"            return;\n"
"          }\n"
"        }\n"
"        return;\n"
"      }\n"
"      else if (window.frames.MSearchResults.searchResults)\n"
"      {\n"
"        var elem = window.frames.MSearchResults.searchResults.NavNext(0);\n"
"        if (elem) elem.focus();\n"
"      }\n"
"    }\n"
"    else if (e.keyCode==27) // Escape out of the search field\n"
"    {\n"
"      this.DOMSearchField().blur();\n"
"      this.DOMPopupSearchResultsWindow().style.display = 'none';\n"
"      this.DOMSearchClose().style.display = 'none';\n"
"      this.lastSearchValue = '';\n"
"      this.Activate(false);\n"
"      return;\n"
"    }\n"
"\n"
"    // strip whitespaces\n"
"    var searchValue = this.DOMSearchField().value.replace(/ +/g, \"\");\n"
"\n"
"    if (searchValue != this.lastSearchValue) // search value has changed\n"
"    {\n"
"      if (searchValue != \"\") // non-empty search\n"
"      {\n"
"        // set timer for search update\n"
"        this.keyTimeout = setTimeout(this.name + '.Search()',\n"
"                                     this.keyTimeoutLength);\n"
"      }\n"
"      else // empty search field\n"
"      {\n"
"        this.DOMPopupSearchResultsWindow().style.display = 'none';\n"
"        this.DOMSearchClose().style.display = 'none';\n"
"        this.lastSearchValue = '';\n"
"      }\n"
"    }\n"
"  }\n"
"\n"
"  this.SelectItemCount = function(id)\n"
"  {\n"
"    var count=0;\n"
"    var win=this.DOMSearchSelectWindow(); \n"
"    for (i=0;i<win.childNodes.length;i++)\n"
"    {\n"
"      var child = win.childNodes[i]; // get span within a\n"
"      if (child.className=='SelectItem')\n"
"      {\n"
"        count++;\n"
"      }\n"
"    }\n"
"    return count;\n"
"  }\n"
"\n"
"  this.SelectItemSet = function(id)\n"
"  {\n"
"    var i,j=0;\n"
"    var win=this.DOMSearchSelectWindow(); \n"
"    for (i=0;i<win.childNodes.length;i++)\n"
"    {\n"
"      var child = win.childNodes[i]; // get span within a\n"
"      if (child.className=='SelectItem')\n"
"      {\n"
"        var node = child.firstChild;\n"
"        if (j==id)\n"
"        {\n"
"          node.innerHTML='&#8226;';\n"
"        }\n"
"        else\n"
"        {\n"
"          node.innerHTML='&#160;';\n"
"        }\n"
"        j++;\n"
"      }\n"
"    }\n"
"  }\n"
"\n"
"  // Called when an search filter selection is made.\n"
"  // set item with index id as the active item\n"
"  this.OnSelectItem = function(id)\n"
"  {\n"
"    this.searchIndex = id;\n"
"    this.SelectItemSet(id);\n"
"    var searchValue = this.DOMSearchField().value.replace(/ +/g, \"\");\n"
"    if (searchValue!=\"\" && this.searchActive) // something was found -> do a search\n"
"    {\n"
"      this.Search();\n"
"    }\n"
"  }\n"
"\n"
"  this.OnSearchSelectKey = function(evt)\n"
"  {\n"
"    var e = (evt) ? evt : window.event; // for IE\n"
"    if (e.keyCode==40 && this.searchIndex<this.SelectItemCount()) // Down\n"
"    {\n"
"      this.searchIndex++;\n"
"      this.OnSelectItem(this.searchIndex);\n"
"    }\n"
"    else if (e.keyCode==38 && this.searchIndex>0) // Up\n"
"    {\n"
"      this.searchIndex--;\n"
"      this.OnSelectItem(this.searchIndex);\n"
"    }\n"
"    else if (e.keyCode==13 || e.keyCode==27)\n"
"    {\n"
"      this.OnSelectItem(this.searchIndex);\n"
"      this.CloseSelectionWindow();\n"
"      this.DOMSearchField().focus();\n"
"    }\n"
"    return false;\n"
"  }\n"
"\n"
"  // --------- Actions\n"
"\n"
"  // Closes the results window.\n"
"  this.CloseResultsWindow = function()\n"
"  {\n"
"    this.DOMPopupSearchResultsWindow().style.display = 'none';\n"
"    this.DOMSearchClose().style.display = 'none';\n"
"    this.Activate(false);\n"
"  }\n"
"\n"
"  this.CloseSelectionWindow = function()\n"
"  {\n"
"    this.DOMSearchSelectWindow().style.display = 'none';\n"
"  }\n"
"\n"
"  // Performs a search.\n"
"  this.Search = function()\n"
"  {\n"
"    this.keyTimeout = 0;\n"
"\n"
"    // strip leading whitespace\n"
"    var searchValue = this.DOMSearchField().value.replace(/^ +/, \"\");\n"
"\n"
"    var code = searchValue.toLowerCase().charCodeAt(0);\n"
"    var idxChar = searchValue.substr(0, 1).toLowerCase();\n"
"    if ( 0xD800 <= code && code <= 0xDBFF && searchValue > 1) // surrogate pair\n"
"    {\n"
"      idxChar = searchValue.substr(0, 2);\n"
"    }\n"
"\n"
"    var resultsPage;\n"
"    var resultsPageWithSearch;\n"
"    var hasResultsPage;\n"
"\n"
"    var idx = indexSectionsWithContent[this.searchIndex].indexOf(idxChar);\n"
"    if (idx!=-1)\n"
"    {\n"
"       var hexCode=idx.toString(16);\n"
"       resultsPage = this.resultsPath + '/' + indexSectionNames[this.searchIndex] + '_' + hexCode + '.html';\n"
"       resultsPageWithSearch = resultsPage+'?'+escape(searchValue);\n"
"       hasResultsPage = true;\n"
"    }\n"
"    else // nothing available for this search term\n"
"    {\n"
"       resultsPage = this.resultsPath + '/nomatches.html';\n"
"       resultsPageWithSearch = resultsPage;\n"
"       hasResultsPage = false;\n"
"    }\n"
"\n"
"    window.frames.MSearchResults.location = resultsPageWithSearch;  \n"
"    var domPopupSearchResultsWindow = this.DOMPopupSearchResultsWindow();\n"
"\n"
"    if (domPopupSearchResultsWindow.style.display!='block')\n"
"    {\n"
"       var domSearchBox = this.DOMSearchBox();\n"
"       this.DOMSearchClose().style.display = 'inline';\n"
"       if (this.insideFrame)\n"
"       {\n"
"         var domPopupSearchResults = this.DOMPopupSearchResults();\n"
"         domPopupSearchResultsWindow.style.position = 'relative';\n"
"         domPopupSearchResultsWindow.style.display  = 'block';\n"
"         var width = document.body.clientWidth - 8; // the -8 is for IE :-(\n"
"         domPopupSearchResultsWindow.style.width    = width + 'px';\n"
"         domPopupSearchResults.style.width          = width + 'px';\n"
"       }\n"
"       else\n"
"       {\n"
"         var domPopupSearchResults = this.DOMPopupSearchResults();\n"
"         var left = getXPos(domSearchBox) + 150; // domSearchBox.offsetWidth;\n"
"         var top  = getYPos(domSearchBox) + 20;  // domSearchBox.offsetHeight + 1;\n"
"         domPopupSearchResultsWindow.style.display = 'block';\n"
"         left -= domPopupSearchResults.offsetWidth;\n"
"         domPopupSearchResultsWindow.style.top     = top  + 'px';\n"
"         domPopupSearchResultsWindow.style.left    = left + 'px';\n"
"       }\n"
"    }\n"
"\n"
"    this.lastSearchValue = searchValue;\n"
"    this.lastResultsPage = resultsPage;\n"
"  }\n"
"\n"
"  // -------- Activation Functions\n"
"\n"
"  // Activates or deactivates the search panel, resetting things to \n"
"  // their default values if necessary. \n"
"  this.Activate = function(isActive)\n"
"  {\n"
"    if (isActive || // open it\n"
"        this.DOMPopupSearchResultsWindow().style.display == 'block' \n"
"       )\n"
"    {\n"
"      this.DOMSearchBox().className = 'MSearchBoxActive';\n"
"\n"
"      var searchField = this.DOMSearchField();\n"
"\n"
"      if (searchField.value == this.searchLabel) // clear \"Search\" term upon entry\n"
"      {  \n"
"        searchField.value = '';  \n"
"        this.searchActive = true;\n"
"      }\n"
"    }\n"
"    else if (!isActive) // directly remove the panel\n"
"    {\n"
"      this.DOMSearchBox().className = 'MSearchBoxInactive';\n"
"      this.DOMSearchField().value   = this.searchLabel;\n"
"      this.searchActive             = false;\n"
"      this.lastSearchValue          = ''\n"
"      this.lastResultsPage          = '';\n"
"    }\n"
"  }\n"
"}\n"
"\n"
"// -----------------------------------------------------------------------\n"
"\n"
"// The class that handles everything on the search results page.\n"
"function SearchResults(name)\n"
"{\n"
"    // The number of matches from the last run of <Search()>.\n"
"    this.lastMatchCount = 0;\n"
"    this.lastKey = 0;\n"
"    this.repeatOn = false;\n"
"\n"
"    // Toggles the visibility of the passed element ID.\n"
"    this.FindChildElement = function(id)\n"
"    {\n"
"      var parentElement = document.getElementById(id);\n"
"      var element = parentElement.firstChild;\n"
"\n"
"      while (element && element!=parentElement)\n"
"      {\n"
"        if (element.nodeName == 'DIV' && element.className == 'SRChildren')\n"
"        {\n"
"          return element;\n"
"        }\n"
"\n"
"        if (element.nodeName == 'DIV' && element.hasChildNodes())\n"
"        {  \n"
"           element = element.firstChild;  \n"
"        }\n"
"        else if (element.nextSibling)\n"
"        {  \n"
"           element = element.nextSibling;  \n"
"        }\n"
"        else\n"
"        {\n"
"          do\n"
"          {\n"
"            element = element.parentNode;\n"
"          }\n"
"          while (element && element!=parentElement && !element.nextSibling);\n"
"\n"
"          if (element && element!=parentElement)\n"
"          {  \n"
"            element = element.nextSibling;  \n"
"          }\n"
"        }\n"
"      }\n"
"    }\n"
"\n"
"    this.Toggle = function(id)\n"
"    {\n"
"      var element = this.FindChildElement(id);\n"
"      if (element)\n"
"      {\n"
"        if (element.style.display == 'block')\n"
"        {\n"
"          element.style.display = 'none';\n"
"        }\n"
"        else\n"
"        {\n"
"          element.style.display = 'block';\n"
"        }\n"
"      }\n"
"    }\n"
"\n"
"    // Searches for the passed string.  If there is no parameter,\n"
"    // it takes it from the URL query.\n"
"    //\n"
"    // Always returns true, since other documents may try to call it\n"
"    // and that may or may not be possible.\n"
"    this.Search = function(search)\n"
"    {\n"
"      if (!search) // get search word from URL\n"
"      {\n"
"        search = window.location.search;\n"
"        search = search.substring(1);  // Remove the leading '?'\n"
"        search = unescape(search);\n"
"      }\n"
"\n"
"      search = search.replace(/^ +/, \"\"); // strip leading spaces\n"
"      search = search.replace(/ +$/, \"\"); // strip trailing spaces\n"
"      search = search.toLowerCase();\n"
"      search = convertToId(search);\n"
"\n"
"      var resultRows = document.getElementsByTagName(\"div\");\n"
"      var matches = 0;\n"
"\n"
"      var i = 0;\n"
"      while (i < resultRows.length)\n"
"      {\n"
"        var row = resultRows.item(i);\n"
"        if (row.className == \"SRResult\")\n"
"        {\n"
"          var rowMatchName = row.id.toLowerCase();\n"
"          rowMatchName = rowMatchName.replace(/^sr\\d*_/, ''); // strip 'sr123_'\n"
"\n"
"          if (search.length<=rowMatchName.length && \n"
"             rowMatchName.substr(0, search.length)==search)\n"
"          {\n"
"            row.style.display = 'block';\n"
"            matches++;\n"
"          }\n"
"          else\n"
"          {\n"
"            row.style.display = 'none';\n"
"          }\n"
"        }\n"
"        i++;\n"
"      }\n"
"      document.getElementById(\"Searching\").style.display='none';\n"
"      if (matches == 0) // no results\n"
"      {\n"
"        document.getElementById(\"NoMatches\").style.display='block';\n"
"      }\n"
"      else // at least one result\n"
"      {\n"
"        document.getElementById(\"NoMatches\").style.display='none';\n"
"      }\n"
"      this.lastMatchCount = matches;\n"
"      return true;\n"
"    }\n"
"\n"
"    // return the first item with index index or higher that is visible\n"
"    this.NavNext = function(index)\n"
"    {\n"
"      var focusItem;\n"
"      while (1)\n"
"      {\n"
"        var focusName = 'Item'+index;\n"
"        focusItem = document.getElementById(focusName);\n"
"        if (focusItem && focusItem.parentNode.parentNode.style.display=='block')\n"
"        {\n"
"          break;\n"
"        }\n"
"        else if (!focusItem) // last element\n"
"        {\n"
"          break;\n"
"        }\n"
"        focusItem=null;\n"
"        index++;\n"
"      }\n"
"      return focusItem;\n"
"    }\n"
"\n"
"    this.NavPrev = function(index)\n"
"    {\n"
"      var focusItem;\n"
"      while (1)\n"
"      {\n"
"        var focusName = 'Item'+index;\n"
"        focusItem = document.getElementById(focusName);\n"
"        if (focusItem && focusItem.parentNode.parentNode.style.display=='block')\n"
"        {\n"
"          break;\n"
"        }\n"
"        else if (!focusItem) // last element\n"
"        {\n"
"          break;\n"
"        }\n"
"        focusItem=null;\n"
"        index--;\n"
"      }\n"
"      return focusItem;\n"
"    }\n"
"\n"
"    this.ProcessKeys = function(e)\n"
"    {\n"
"      if (e.type == \"keydown\") \n"
"      {\n"
"        this.repeatOn = false;\n"
"        this.lastKey = e.keyCode;\n"
"      }\n"
"      else if (e.type == \"keypress\")\n"
"      {\n"
"        if (!this.repeatOn)\n"
"        {\n"
"          if (this.lastKey) this.repeatOn = true;\n"
"          return false; // ignore first keypress after keydown\n"
"        }\n"
"      }\n"
"      else if (e.type == \"keyup\")\n"
"      {\n"
"        this.lastKey = 0;\n"
"        this.repeatOn = false;\n"
"      }\n"
"      return this.lastKey!=0;\n"
"    }\n"
"\n"
"    this.Nav = function(evt,itemIndex) \n"
"    {\n"
"      var e  = (evt) ? evt : window.event; // for IE\n"
"      if (e.keyCode==13) return true;\n"
"      if (!this.ProcessKeys(e)) return false;\n"
"\n"
"      if (this.lastKey==38) // Up\n"
"      {\n"
"        var newIndex = itemIndex-1;\n"
"        var focusItem = this.NavPrev(newIndex);\n"
"        if (focusItem)\n"
"        {\n"
"          var child = this.FindChildElement(focusItem.parentNode.parentNode.id);\n"
"          if (child && child.style.display == 'block') // children visible\n"
"          { \n"
"            var n=0;\n"
"            var tmpElem;\n"
"            while (1) // search for last child\n"
"            {\n"
"              tmpElem = document.getElementById('Item'+newIndex+'_c'+n);\n"
"              if (tmpElem)\n"
"              {\n"
"                focusItem = tmpElem;\n"
"              }\n"
"              else // found it!\n"
"              {\n"
"                break;\n"
"              }\n"
"              n++;\n"
"            }\n"
"          }\n"
"        }\n"
"        if (focusItem)\n"
"        {\n"
"          focusItem.focus();\n"
"        }\n"
"        else // return focus to search field\n"
"        {\n"
"           parent.document.getElementById(\"MSearchField\").focus();\n"
"        }\n"
"      }\n"
"      else if (this.lastKey==40) // Down\n"
"      {\n"
"        var newIndex = itemIndex+1;\n"
"        var focusItem;\n"
"        var item = document.getElementById('Item'+itemIndex);\n"
"        var elem = this.FindChildElement(item.parentNode.parentNode.id);\n"
"        if (elem && elem.style.display == 'block') // children visible\n"
"        {\n"
"          focusItem = document.getElementById('Item'+itemIndex+'_c0');\n"
"        }\n"
"        if (!focusItem) focusItem = this.NavNext(newIndex);\n"
"        if (focusItem)  focusItem.focus();\n"
"      }\n"
"      else if (this.lastKey==39) // Right\n"
"      {\n"
"        var item = document.getElementById('Item'+itemIndex);\n"
"        var elem = this.FindChildElement(item.parentNode.parentNode.id);\n"
"        if (elem) elem.style.display = 'block';\n"
"      }\n"
"      else if (this.lastKey==37) // Left\n"
"      {\n"
"        var item = document.getElementById('Item'+itemIndex);\n"
"        var elem = this.FindChildElement(item.parentNode.parentNode.id);\n"
"        if (elem) elem.style.display = 'none';\n"
"      }\n"
"      else if (this.lastKey==27) // Escape\n"
"      {\n"
"        parent.searchBox.CloseResultsWindow();\n"
"        parent.document.getElementById(\"MSearchField\").focus();\n"
"      }\n"
"      else if (this.lastKey==13) // Enter\n"
"      {\n"
"        return true;\n"
"      }\n"
"      return false;\n"
"    }\n"
"\n"
"    this.NavChild = function(evt,itemIndex,childIndex)\n"
"    {\n"
"      var e  = (evt) ? evt : window.event; // for IE\n"
"      if (e.keyCode==13) return true;\n"
"      if (!this.ProcessKeys(e)) return false;\n"
"\n"
"      if (this.lastKey==38) // Up\n"
"      {\n"
"        if (childIndex>0)\n"
"        {\n"
"          var newIndex = childIndex-1;\n"
"          document.getElementById('Item'+itemIndex+'_c'+newIndex).focus();\n"
"        }\n"
"        else // already at first child, jump to parent\n"
"        {\n"
"          document.getElementById('Item'+itemIndex).focus();\n"
"        }\n"
"      }\n"
"      else if (this.lastKey==40) // Down\n"
"      {\n"
"        var newIndex = childIndex+1;\n"
"        var elem = document.getElementById('Item'+itemIndex+'_c'+newIndex);\n"
"        if (!elem) // last child, jump to parent next parent\n"
"        {\n"
"          elem = this.NavNext(itemIndex+1);\n"
"        }\n"
"        if (elem)\n"
"        {\n"
"          elem.focus();\n"
"        } \n"
"      }\n"
"      else if (this.lastKey==27) // Escape\n"
"      {\n"
"        parent.searchBox.CloseResultsWindow();\n"
"        parent.document.getElementById(\"MSearchField\").focus();\n"
"      }\n"
"      else if (this.lastKey==13) // Enter\n"
"      {\n"
"        return true;\n"
"      }\n"
"      return false;\n"
"    }\n"
"}\n"
"\n"
"function setKeyActions(elem,action)\n"
"{\n"
"  elem.setAttribute('onkeydown',action);\n"
"  elem.setAttribute('onkeypress',action);\n"
"  elem.setAttribute('onkeyup',action);\n"
"}\n"
"\n"
"function setClassAttr(elem,attr)\n"
"{\n"
"  elem.setAttribute('class',attr);\n"
"  elem.setAttribute('className',attr);\n"
"}\n"
"\n"
"function createResults()\n"
"{\n"
"  var results = document.getElementById(\"SRResults\");\n"
"  for (var e=0; e<searchData.length; e++)\n"
"  {\n"
"    var id = searchData[e][0];\n"
"    var srResult = document.createElement('div');\n"
"    srResult.setAttribute('id','SR_'+id);\n"
"    setClassAttr(srResult,'SRResult');\n"
"    var srEntry = document.createElement('div');\n"
"    setClassAttr(srEntry,'SREntry');\n"
"    var srLink = document.createElement('a');\n"
"    srLink.setAttribute('id','Item'+e);\n"
"    setKeyActions(srLink,'return searchResults.Nav(event,'+e+')');\n"
"    setClassAttr(srLink,'SRSymbol');\n"
"    srLink.innerHTML = searchData[e][1][0];\n"
"    srEntry.appendChild(srLink);\n"
"    if (searchData[e][1].length==2) // single result\n"
"    {\n"
"      srLink.setAttribute('href',searchData[e][1][1][0]);\n"
"      if (searchData[e][1][1][1])\n"
"      {\n"
"       srLink.setAttribute('target','_parent');\n"
"      }\n"
"      var srScope = document.createElement('span');\n"
"      setClassAttr(srScope,'SRScope');\n"
"      srScope.innerHTML = searchData[e][1][1][2];\n"
"      srEntry.appendChild(srScope);\n"
"    }\n"
"    else // multiple results\n"
"    {\n"
"      srLink.setAttribute('href','javascript:searchResults.Toggle(\"SR_'+id+'\")');\n"
"      var srChildren = document.createElement('div');\n"
"      setClassAttr(srChildren,'SRChildren');\n"
"      for (var c=0; c<searchData[e][1].length-1; c++)\n"
"      {\n"
"        var srChild = document.createElement('a');\n"
"        srChild.setAttribute('id','Item'+e+'_c'+c);\n"
"        setKeyActions(srChild,'return searchResults.NavChild(event,'+e+','+c+')');\n"
"        setClassAttr(srChild,'SRScope');\n"
"        srChild.setAttribute('href',searchData[e][1][c+1][0]);\n"
"        if (searchData[e][1][c+1][1])\n"
"        {\n"
"         srChild.setAttribute('target','_parent');\n"
"        }\n"
"        srChild.innerHTML = searchData[e][1][c+1][2];\n"
"        srChildren.appendChild(srChild);\n"
"      }\n"
"      srEntry.appendChild(srChildren);\n"
"    }\n"
"    srResult.appendChild(srEntry);\n"
"    results.appendChild(srResult);\n"
"  }\n"
"}\n"
"\n"
