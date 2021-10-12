var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onload);
var direction;

function onload(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function submitForm(){
    const rbs = document.querySelectorAll('input[name="direction"]');
    direction;
    for (const rb of rbs) {
        if (rb.checked) {
            direction = rb.value;
            break;
        }
    }

    document.getElementById("motor-state").innerHTML = "motor spinning...";
    document.getElementById("motor-state").style.color = "blue";
    if (direction=="CW"){
        document.getElementById("gear").classList.add("spin");
    }
    else{
        document.getElementById("gear").classList.add("spin-back");
    }
    
    var steps = document.getElementById("steps").value;
    websocket.send(steps+"&"+direction);
}

function onMessage(event) {
    console.log(event.data);
    direction = event.data;
    if (direction=="stop"){ 
      document.getElementById("motor-state").innerHTML = "motor stopped"
      document.getElementById("motor-state").style.color = "red";
      document.getElementById("gear").classList.remove("spin", "spin-back");
    }
    else if(direction=="CW" || direction=="CCW"){
        document.getElementById("motor-state").innerHTML = "motor spinning...";
        document.getElementById("motor-state").style.color = "blue";
        if (direction=="CW"){
            document.getElementById("gear").classList.add("spin");
        }
        else{
            document.getElementById("gear").classList.add("spin-back");
        }
    }
}