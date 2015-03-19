if (!String.prototype.startsWith) {
	Object.defineProperty(String.prototype, 'startsWith', {
		enumerable: false,
		configurable: false,
		writable: false,
		value: function(searchString, position) {
			position = position || 0;
			return this.lastIndexOf(searchString, position) === position;
		}
	});
}

navigator.plugins.refresh(true);

(function(){
	var LOCKER_DEFAULT_NAMESPACE = "items",
		LOCKER_CTRL_NAMESPACE = "itemctrl",
		LOCKER_SEPARATOR = '.',
		app = angular.module('store', ['angular-locker', 'angular-velocity']).config(['lockerProvider', function config(lockerProvider) {
			lockerProvider.setDefaultDriver('local')
				.setDefaultNamespace(LOCKER_DEFAULT_NAMESPACE)
				.setSeparator(LOCKER_SEPARATOR)
				.setEventsEnabled(true);
		}]);

	app.controller('StoreController', ['$rootScope', '$scope', 'locker', 'optionsService', function($rootScope, $scope, locker, options) {
		/* -------------------------------- Initialization -------------------------------- */
		this.options = options;
		var me = this,
			items = [];
		this.items = items;

		var lockerItems = locker.all();
		Object.keys(lockerItems).forEach(function(key,index) {
			//key = the name of the object key
			//index = the ordinal position of the key within the object
			items.push(angular.fromJson(lockerItems[key]));
		});

		this.addItem = function(name) {
			var newItem = {
				"name": name,
				"enabled":false,
				"keyTheme": "Flat-Dark",
				"keys": "1 2 3 4 5 6 7 8 9 0 Ctrl+1 Ctrl+2 Ctrl+3 Ctrl+4 Ctrl+5 Ctrl+6 Ctrl+7 Ctrl+8 Ctrl+9 Ctrl+0 Alt+1 Alt+2 Alt+3 Alt+4 Alt+5 Alt+6 Alt+7 Alt+8 Alt+9 Alt+0"
			};

			if (locker.add(name, newItem))	//conditional put
				items.push(newItem);

			return newItem;
		};
		this.addItem('Default');
		
		this.activeItem = lockerItems["Default"];
		locker.namespace(LOCKER_CTRL_NAMESPACE).put("active", "Default");
		
		var animationDurations = {
			in: 5,
			out: 3000
		}
		this.animationDuration = animationDurations["in"];
		
		/* -------------------------------- key event synchro -------------------------------- */
		var MODKEY_FLAGS = {	// pretty old school eh?
			CTRL: 1,
			SHIFT: 2,
			ALT: 4
		}

		this.isCurrentKeyComboValid = false;
		var isCurrentKeyComboValid = function() {
			if (!$scope.keyVis.isCharDown)
				return false;
			
			var char = $scope.keyVis.Char;
			if (!keycombos[char])
				return false;
			
			var flags = 0;
			if ($scope.keyVis.Ctrl)
				flags |= MODKEY_FLAGS.CTRL;
			
			if ($scope.keyVis.Shift)
				flags |= MODKEY_FLAGS.SHIFT;
			
			if ($scope.keyVis.Alt)
				flags |= MODKEY_FLAGS.ALT;
			
			return (keycombos[char][flags] === true);
		}
		
		var keycombos = {},
			updateKeyCombos = function() {

				var str_length = me.activeItem.keys.length;
				//TODO: Abort on empty? all keys on empty?

				keycombos_s = (me.activeItem.keys).split(" ");

				for (var i = 0, length = keycombos_s.length; i < length; i++) {
					var keycombo_s = keycombos_s[i],
						split = keycombo_s.split("+");

					var flag = 0,
						key = false;

					for (var j=0; j < split.length; j++) {
						var comboItem = split[j].toUpperCase();
						if (comboItem === "CTRL")
							flag |= MODKEY_FLAGS.CTRL;
						else if (comboItem === "SHIFT")
							flag |= MODKEY_FLAGS.SHIFT;
						else if (comboItem === "ALT")
							flag |= MODKEY_FLAGS.ALT;
						else if (!key)
							key = comboItem;
						else
							console.error("Unexpected element in KeyCombo", comboItem);

					}
					if (!keycombos[key])
						keycombos[key] = {};

					keycombos[key][flag] = true;
				}
				console.log(keycombos);
			};

		updateKeyCombos();
		
		$scope.keyVis = {"Char":false};
		var keyCaptureId = -1,
			updateKeyEventCapture = function() {
				if (keyCaptureId == -1) {	// && (me.activeItem.enabled)
					plugin().startKeyCapture("unused",
											 function(result) {
						keyCaptureId = result;
					},
											 function(result) {
						var obj;
						try {
							obj = JSON.parse(result);
						} catch (e) {
							console.error(e.message + result);
							return;
						}
						//console.log(obj.Char, obj.MakeBreak);
						var isDown = (obj.MakeBreak === "[M]");
						if (obj.Char == "CTRL")
							$scope.keyVis.Ctrl = ((isDown) ? true : false);
						else if (obj.Char === "SHIFT")
							$scope.keyVis.Shift = ((isDown === "[M]") ? true : false);
						else if (obj.Char == "ALT")
							$scope.keyVis.Alt = ((isDown === "[M]") ? true : false);
						else {
							if (isDown) {
								$scope.keyVis.Char = obj.Char;	// setting char to false would break fadeOut
								$scope.keyVis.isCharDown = (obj.MakeBreak === "[M]");
								// do only here to eliminate key->modifier->hotkey
								var isValid = isCurrentKeyComboValid();
								me.animationDuration = (isValid) ? animationDurations["in"] : animationDurations["out"];
								me.isCurrentKeyComboValid = isValid;
							} else {
								
							}
							$scope.$apply();
						}

					});

				}
			};
		updateKeyEventCapture();

		/* -------------------------------- LocalStorage Update -------------------------------- */
		// locker events are only for local monitoring, use the normal event for external changes

		window.addEventListener("storage", function(e) {
			//console.log("Storage changed", e);
			var key = e.key;
			if (!key.startsWith(LOCKER_DEFAULT_NAMESPACE + LOCKER_SEPARATOR))
				return;

			var oldItem = JSON.parse(e.oldValue),
				newItem = JSON.parse(e.newValue);

			if (!lockerItems[newItem.name]) {
				console.warn("Storage Event listener: Should never get here");
				lockerItems[newItem.name] = newItem;
				items.push(newItem);
			} else {	// via manual diff update we preserve the link of lockerItems/items/currentItem/activeItem
				$.each(newItem, function(k,v) {
					if (typeof oldItem[k] === "undefined")
						return true; // that's continue; return false; is break

					if (v !== oldItem[k]) {
						lockerItems[newItem.name][k] = v;
						console.log("updated value: ", newItem.name, k , v);
					}
				});
			}
			if (newItem.name === me.activeItem.name)
				updateKeyCombos();

		}, false);

		/*$rootScope.$on('locker.item.updated', function (e, payload) {
			console.log('locker.item.updated (local monitoring)', payload);
		});*/

		/* -------------------------------- Game Info -------------------------------- */

		//updateKeyEventCapture();
		var handleGameInfo = function(gameInfo) {
			if (!gameInfo)
				return;

			var name = gameInfo.title,
				isInGame = (gameInfo.isRunning) && (gameInfo.isInFocus),
				item;

			if (!isInGame)
				name = "Default";

			if (!lockerItems[name]) {
				item = me.addItem(name);
			} else {
				item = lockerItems[name];
			}
			me.activeItem = item;
			locker.namespace(LOCKER_CTRL_NAMESPACE).put("active", item.name);
			updateKeyCombos();
		};

		overwolf.games.getRunningGameInfo (function(result) {
			console.log("GetRunningGameInfo", result);
			handleGameInfo(result);
		});

		overwolf.games.onGameInfoUpdated.addListener(function(result) {
			console.log("onGameInfoUpdated", result);
			if (!result)
				return;

			if (result.gameChanged || result.runningChanged || result.focusChanged)
				handleGameInfo(result.gameInfo);
		});
	}]);

	// non angular
	/*overwolf.windows.obtainDeclaredWindow("index", function(result) {
		if (! (result && (result.status === "success")))
			return;

		var odkWindow = result.window;
		overwolf.windows.restore(odkWindow.id, function(result) {
			//console.log("index restore", result.status);
		});
	});*/

	// Hotkeys
	/*
	var hotkeys = {}
	function HotkeySetting(name) {
		return function(result) {
			if (!(result && (result.status === "success"))) {
				console.error("Could not get Hotkey setting", name);
			} else {
				hotkeys[name] = result.hotkey;
				console.log("result", name, result.hotkey);
			}
			if (name === "Key_2")
				console.log("Hotkey setting update done.", hotkeys);
		}
	};

	function updateHotkeys() {
		for (var i=1; i<3; i++) {
			overwolf.settings.getHotKey(
				"Key_"+i,
				HotkeySetting("Key_"+i)
			);
		};
	};
	updateHotkeys();

	function Hotkey(name) {
		return function() {
			overwolf.utils.sendKeyStroke(hotkeys[name]);
			//alert(name + " is running!")
			$("#toggleCapture").html(hotkeys[name]);
		}
	};

	for (var i=1; i<3; i++) {
		overwolf.settings.registerHotKey(
			"Key_"+i,
			Hotkey("Key_"+i)
		);
	};*/

})();

function plugin() {
	return document.querySelector('#plugin');
}

$(function() {
	console.log( "ready!" );

	if (plugin() == null) {
		console.error("Plugin couldn't be loaded??");
		return;
	} else {
		console.log("Plugin was loaded!");
	}
});

var map = {
	command   : '⌘',
	shift     : '⇧',
	left      : '←',
	right     : '→',
	up        : '↑',
	down      : '↓',
	'return'  : '↩',
	backspace : '⌫'
};