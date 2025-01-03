const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Управление двигателем</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      font-size: 20px;
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }
    .container {
      display: flex;
      flex-direction: column;
      align-items: center;
      width: 100%;
      padding: 20px;
      box-sizing: border-box;
    }
    .tumbler-container {
        display: flex;
        justify-content: space-between;
        align-items: center;
        width: 300px;
        padding: 10px;
        margin: 1px 0;
        box-sizing: border-box;
        background-color: #f9f9f9;
        border-radius: 5px;
        box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
    }
    .switch {
      position: relative;
      display: inline-block;
      width: 60px;
      height: 34px;
    }
    .switch input {
      opacity: 0;
      width: 0;
      height: 0;
    }
    .slider {
      position: absolute;
      cursor: pointer;
      top: 0;
      left: 0;
      right: 0;
      bottom: 0;
      background-color: #ccc;
      transition: .4s;
      border-radius: 34px;
    }
    .slider:before {
      position: absolute;
      content: "";
      height: 26px;
      width: 26px;
      left: 4px;
      bottom: 4px;
      background-color: white;
      transition: .4s;
      border-radius: 50%;
    }
    input:checked + .slider {
      background-color: #4CAF50;
    }
    input:checked + .slider:before {
      transform: translateX(26px);
    }
    .buttons {
      display: flex;
      justify-content: center;
      margin-top: 20px;
      width: 100%;
    }
    .buttons button {
      padding: 10px 20px;
      font-size: 18px;
      cursor: pointer;
      border: none;
      border-radius: 5px;
    }
    .start {
      background-color: #4CAF50;
      color: white;
    }
    .stop {
      background-color: #F44336;
      color: white;
    }
  </style>
</head>
<body>
  <div class="container">
    <div class="tumbler-container">
      <label class="switch">
        <input type="checkbox" id="motorSwitch" onchange="toggleMotor()">
        <span class="slider"></span>
      </label>
      <span id="motorLabel">Левый двигатель</span>
    </div>

    <div class="tumbler-container">
      <label class="switch">
        <input type="checkbox" id="directionSwitch" onchange="toggleDirection()">
        <span class="slider"></span>
      </label>
      <span id="directionLabel">По часовой</span>
    </div>

    <div class="buttons">
      <button id="startStopButton" onclick="startStopMotor()">Старт</button>
    </div>
  </div>

  <script>
    let motorRunning = false;

	function toggleMotor() {
	  const motor = document.getElementById('motorSwitch').checked ? 1 : 0;
	  
	  // Временно отключаем тумблер
	  document.getElementById('motorSwitch').disabled = true;

	  // Меняем текст без отправки запроса
	  document.getElementById('motorLabel').textContent = motor ? 'Правый двигатель' : 'Левый двигатель';

	  // Отправляем запрос на сервер
	  fetch('/select?motor=' + motor)
		.then(() => {
		  // Включаем тумблер обратно, когда запрос завершится
		  document.getElementById('motorSwitch').disabled = false;
		  console.log('Двигатель выбран: ' + motor);
		})
		.catch(() => {
		  // Если запрос не удался, восстанавливаем состояние тумблера
		  document.getElementById('motorSwitch').checked = !motor;
		  document.getElementById('motorSwitch').disabled = false;
		});
	}

    function toggleDirection() {
      const direction = document.getElementById('directionSwitch').checked ? 'CW' : 'CCW';
      document.getElementById('directionLabel').textContent = direction === 'CW' ? 'По часовой' : 'Против часовой';
      fetch('/direction?dir=' + direction)
        .then(() => console.log('Направление: ' + direction));
    }

    function startStopMotor() {
      fetch('/startstop')
        .then(response => response.text())
        .then(text => {
          motorRunning = !motorRunning;
          document.getElementById('startStopButton').textContent = motorRunning ? 'Стоп' : 'Старт';
          document.getElementById('startStopButton').className = motorRunning ? 'stop' : 'start';
          console.log(text);
        });
    }

    function updateStatus() {
      fetch('/status')
        .then(response => response.json())
        .then(data => {
          document.getElementById('motorSwitch').checked = data.selectedMotor === 1;
          document.getElementById('motorLabel').textContent = data.selectedMotor === 1 ? 'Правый двигатель' : 'Левый двигатель';

          document.getElementById('directionSwitch').checked = data.motorDirection === 'CW';
          document.getElementById('directionLabel').textContent = data.motorDirection === 'CW' ? 'По часовой' : 'Против часовой';

          motorRunning = data.motorRunning;
          document.getElementById('startStopButton').textContent = motorRunning ? 'Стоп' : 'Старт';
          document.getElementById('startStopButton').className = motorRunning ? 'stop' : 'start';
        });
    }

    setInterval(updateStatus, 1000);
    window.onload = updateStatus;
  </script>
</body>
</html>
)rawliteral";
