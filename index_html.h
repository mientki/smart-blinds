String INDEX_HTML = R"(<!DOCTYPE html>
<html>
<head>
  <meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate' />
  <meta http-equiv='Pragma' content='no-cache' />
  <meta http-equiv='Expires' content='0' />
  <title>{NAME}</title>
  <link rel='stylesheet' href='https://unpkg.com/onsenui/css/onsenui.css'>
  <link rel='stylesheet' href='https://unpkg.com/onsenui/css/onsen-css-components.min.css'>
  <script src='https://unpkg.com/onsenui/js/onsenui.min.js'></script>
  <script src='https://unpkg.com/jquery/dist/jquery.min.js'></script>
  <script>
  var cversion = '{VERSION}';
  var wsUri = 'ws://'+location.host+':81/';
  var repo = 'motor-on-roller-blind-ws';

  window.fn = {};
  window.fn.open = function() {
    var menu = document.getElementById('menu');
    menu.open();
  };

  window.fn.load = function(page) {
    var content = document.getElementById('content');
    var menu = document.getElementById('menu');
    content.load(page)
      .then(menu.close.bind(menu)).then(setActions());
  };

  var gotoPos = function(percent){
    doSend(percent);
  };
  var instr = function(action){
    doSend('('+action+')');
  };

  var setActions = function(){
    doSend('(update)');
    $.get('https://api.github.com/repos/nidayand/'+repo+'/releases', function(data){
      if (data.length>0 && data[0].tag_name !== cversion){
        $('#cversion').text(cversion);
        $('#nversion').text(data[0].tag_name);
        $('#update-card').show();
      }
    });

    setTimeout(function(){
      $('#arrow-close').on('click', function(){$('#setrange').val(100);gotoPos(100);});
      $('#arrow-open').on('click', function(){$('#setrange').val(0);gotoPos(0);});
      $('#setrange').on('change', function(){gotoPos($('#setrange').val())});

      $('#arrow-up-man').on('click', function(){instr('-1')});
      $('#arrow-down-man').on('click', function(){instr('1')});
      $('#arrow-stop-man').on('click', function(){instr('0')});
      $('#set-start').on('click', function(){instr('start')});
      $('#set-max').on('click', function(){instr('max');});

    }, 200);
  };
  $(document).ready(function(){
    setActions();
  });

  var websocket;
  var timeOut;
  function retry(){
    clearTimeout(timeOut);
    timeOut = setTimeout(function(){
      websocket=null; init();},5000);
  };
  function init(){
    ons.notification.toast({message: 'Connecting...', timeout: 1000});
    try{
      websocket = new WebSocket(wsUri);
      websocket.onclose = function () {};
      websocket.onerror = function(evt) {
        ons.notification.toast({message: 'Cannot connect to device', timeout: 2000});
        retry();
      };
      websocket.onopen = function(evt) {
        ons.notification.toast({message: 'Connected to device', timeout: 2000});
        setTimeout(function(){doSend('(update)');}, 1000);
      };
      websocket.onclose = function(evt) {
        ons.notification.toast({message: 'Disconnected. Retrying', timeout: 2000});
        retry();
      };
      websocket.onmessage = function(evt) {
        try{
          var msg = JSON.parse(evt.data);
          if (typeof msg.position !== 'undefined'){
            $('#pbar').attr('value', msg.position);
          };
          if (typeof msg.set !== 'undefined'){
            $('#setrange').val(msg.set);
          };
        } catch(err){}
      };
    } catch (e){
      ons.notification.toast({message: 'Cannot connect to device. Retrying...', timeout: 2000});
      retry();
    };
  };
  function doSend(msg){
    if (websocket && websocket.readyState == 1){
      websocket.send(msg);
    }
  };
  window.addEventListener('load', init, false);
  window.onbeforeunload = function() {
    if (websocket && websocket.readyState == 1){
      websocket.close();
    };
  };
  </script>
</head>
<body>

<ons-splitter>
  <ons-splitter-side id='menu' side='left' width='220px' collapse swipeable>
    <ons-page>
      <ons-list>
        <ons-list-item onclick='fn.load("home.html")' tappable>
          Home
        </ons-list-item>
        <ons-list-item onclick='fn.load("settings.html")' tappable>
          Ustawienia
        </ons-list-item>
        <ons-list-item onclick='fn.load("about.html")' tappable>
          About
        </ons-list-item>
      </ons-list>
    </ons-page>
  </ons-splitter-side>
  <ons-splitter-content id='content' page='home.html'></ons-splitter-content>
</ons-splitter>

<template id='home.html'>
  <ons-page>
    <ons-toolbar>
      <div class='left'>
        <ons-toolbar-button onclick='fn.open()'>
          <ons-icon icon='md-menu'></ons-icon>
        </ons-toolbar-button>
      </div>
      <div class='center'>
        {NAME}
      </div>
    </ons-toolbar>
<ons-card>
    <div class='title'>Ustaw pozycję</div>
  <div class='content'><p>Przesuń suwak do pożądanej pozycji lub użyj strzałek, aby otworzyć lub zamknąć całkowicie roletę.</p></div>
  <ons-row>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
      </ons-col>
      <ons-col>
         <ons-progress-bar id='pbar' value='75'></ons-progress-bar>
      </ons-col>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
      </ons-col>
  </ons-row>
    <ons-row>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
        <ons-icon id='arrow-open' icon='fa-arrow-up' size='2x'></ons-icon>
      </ons-col>
      <ons-col>
        <ons-range id='setrange' style='width: 100%;' value='25'></ons-range>
      </ons-col>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
        <ons-icon id='arrow-close' icon='fa-arrow-down' size='2x'></ons-icon>
      </ons-col>
    </ons-row>

    </ons-card>
    <ons-card id='update-card' style='display:none'>
      <div class='title'>Update available</div>
      <div class='content'>Twoja wersja to <span id='cversion'></span>. Dostępna jest nowa wersja <span id='nversion'></span>. Odwiedź stronę <a href='https://github.com/mientki/smart-blinds/releases'>GitHub</a> aby pobrać nową wersję.</div>
    </ons-card>
  </ons-page>
</template>

<template id='settings.html'>
  <ons-page>
    <ons-toolbar>
      <div class='left'>
        <ons-toolbar-button onclick='fn.open()'>
          <ons-icon icon='md-menu'></ons-icon>
        </ons-toolbar-button>
      </div>
      <div class='center'>
        Settings
      </div>
    </ons-toolbar>
  <ons-card>
    <div class='title'>Instrukcja</div>
    <div class='content'>
    <p>
    <ol>
      <li>Użyj strzałek i przycisku stop, aby przejść do najwyższej pozycji, aż roleta się otworzy</li>
      <li>Naciśnij przycisk START</li>
      <li>Użyj strzałki w dół, aby przejść do maksymalnej pozycji zamkniętej</li>
      <li>Naciśnij przycisk MAX</li>
      <li>Kalibracja jest zakończona.</li>
    </ol>
    </p>
  </div>
  </ons-card>
  <ons-card>
    <div class='title'>Control</div>
    <ons-row style='width:100%'>
      <ons-col style='text-align:center'><ons-icon id='arrow-up-man' icon='fa-arrow-up' size='2x'></ons-icon></ons-col>
      <ons-col style='text-align:center'><ons-icon id='arrow-stop-man' icon='fa-stop' size='2x'></ons-icon></ons-col>
      <ons-col style='text-align:center'><ons-icon id='arrow-down-man' icon='fa-arrow-down' size='2x'></ons-icon></ons-col>
    </ons-row>
  </ons-card>
  <ons-card>
    <div class='title'>Store</div>
    <ons-row style='width:100%'>
      <ons-col style='text-align:center'><ons-button id='set-start'>Ustaw Start</ons-button></ons-col>
      <ons-col style='text-align:center'>&nbsp;</ons-col>
      <ons-col style='text-align:center'><ons-button id='set-max'>Ustaw Max</ons-button></ons-col>
    </ons-row>
  </ons-card>
  </ons-page>
</template>

<template id='about.html'>
  <ons-page>
    <ons-toolbar>
      <div class='left'>
        <ons-toolbar-button onclick='fn.open()'>
          <ons-icon icon='md-menu'></ons-icon>
        </ons-toolbar-button>
      </div>
      <div class='center'>
        About
      </div>
    </ons-toolbar>
  <ons-card>
    <div class='title'>Motor on a roller blind</div>
    <div class='content'>
    <p>
      <ul>
        <li>3d print files and instructions: <a href='https://www.thingiverse.com/thing:2392856' target='_blank'>https://www.thingiverse.com/thing:2392856</a></li>
        <li>Github: <a href='https://github.com/nidayand/motor-on-roller-blind-ws' target='_blank'>https://github.com/nidayand/motor-on-roller-blind-ws</a></li>
        <li>Licensed under <a href='https://raw.githubusercontent.com/nidayand/motor-on-roller-blind-ws/master/LICENSE' target='_blank'>MIT License</a></li>
      </ul>
    </p>
  </div>
  </ons-card>
  </ons-page>
</template>

</body>
</html>
)";
