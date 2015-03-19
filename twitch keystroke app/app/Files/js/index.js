(function() {
	var LOCKER_DEFAULT_NAMESPACE = "items",
		LOCKER_CTRL_NAMESPACE = "itemctrl",
		LOCKER_SEPARATOR = '.',
		app = angular.module('store', ['angular-locker']).config(['lockerProvider', function config(lockerProvider) {
		lockerProvider.setDefaultDriver('local')
			.setDefaultNamespace(LOCKER_DEFAULT_NAMESPACE)
			.setSeparator(LOCKER_SEPARATOR)
			.setEventsEnabled(true);
	}]);

	app.controller('StoreController', ['$rootScope','locker', 'optionsService', function($rootScope,locker, options) {
		this.options = options;
		var items = [];
		//$scope.items = items;
		this.items = items;

		//this.selected = 0;
		//$scope.selected = this.selected;

		var lockerItems = locker.all();
		Object.keys(lockerItems).forEach(function(key,index) {
			//key = the name of the object key
			//index = the ordinal position of the key within the object
			items.push(angular.fromJson(lockerItems[key]));
		});
		this.currentItem = items[0];
				
		this.activeItem	 = locker.namespace(LOCKER_CTRL_NAMESPACE).get("active");
		/* -------------------------------- LocalStorage Update -------------------------------- */
		
		// locker events are only for local monitoring, use the normal event for external changes
		window.addEventListener("storage", function(e) {
			console.log("Storage changed (to be used for current/running key)", e);
		}, false);
		
		// fired when an item's value changes to something new
		$rootScope.$on('locker.item.updated', function (e, payload) {
			// payload is equal to:
			/*{
				driver: 'local', // the driver that was set when the event was fired
				namespace: 'locker', // the namespace that was set when the event was fired
				key: 'foo', // the key that was updated
				oldValue: 'bar', // the value that was set before the item was updated
				newValue: 'baz' // the new value that the item was updated to
			}*/
			console.log('locker.item.updated (local monitoring)', payload);
		});
		
		// fired when a new item is added to storage
		$rootScope.$on('locker.item.added', function (e, payload) {
			// payload is equal to:
			/*{
				driver: 'local', // the driver that was set when the event was fired
				namespace: 'locker', // the namespace that was set when the event was fired
				key: 'foo', // the key that was added
				value: 'bar' // the value that was added
			}*/
			console.log('locker.item.added (local monitoring)', payload);
		});
		
		this.saveToStorage = function() {
			console.log("saveToStorage");
			locker.put(this.currentItem.name, this.currentItem);
		}
		
		/*this.keyThemes = {
			"Flat-Dark": { class: "dark" },
			"Flat-Light": { class: "light" },
			"3D": { class: "keyThreeDee" }
		}*/
		/*this.addItem = function(name) {
			var newItem = {
				"name": name,
				"enabled":true,
				"keys": "1 2 3 4 5 6 7 8 9 0 Strg+1 Strg+2 Strg+3 Strg+4 Strg+5 Strg+6 Strg+7 Strg+8 Strg+9 Strg+0 Alt+1 Alt+2 Alt+3 Alt+4 Alt+5 Alt+6 Alt+7 Alt+8 Alt+9 Alt+0"
			};

			if (locker.add(name, newItem))	//conditional put
				items.push(newItem);

			return newItem;
		};*/

		/*
	this.selectItem = function(name) {
		//$scope.currentItem = lockerItems[name];
		me.currentItem = lockerItems[name];
	};*/

		

	}]);
	

	
})();

$(function() {
	console.log( "ready!" );
	overwolf.windows.obtainDeclaredWindow("sWindow", function(result) {
		if (! (result && (result.status === "success")))
			return;

		var odkWindow = result.window;
		overwolf.windows.restore(odkWindow.id, function(result) {
			//console.log("index restore", result.status);
		});
	});

});

