angular.module('store').factory('optionsService', function() {

	var options  = {
		keyThemes : {
			"Flat-Dark": { class: "dark" },
			"Flat-Light": { class: "light" },
			"3D": { class: "keyThreeDee" }
		}
	};

	return options;
})