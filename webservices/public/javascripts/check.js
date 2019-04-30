(function() {
    'use strict';

    window.onload = function() {
        const updateUrl = "/home/updateStatus";
        const checkUrl = "/home/checkStatus";
        const installUrl = "/home/install"
        var data = {
            checked: false,
            service: "",
        };

        // On window load update each checkbox. Change to install button if appropriate.
        $(".checkbox").each(function() {
            var current = this;
            data.checked = current.checked;
            data.service = current.id;
            $(current).prop('disabled', true);
            $.post(checkUrl, data) 
                .done(function(data, status, response){
                    var response = response.responseText;
                    if (response == "enabled") {
                        $(current).prop('checked', true);
                    } else if (response == "disabled") {
                        $(current).prop('checked', false);
                    } else if (response == "not installed"){
                        $(current).parent().hide();
                        $(current).parent().next().removeAttr('hidden');
                    }
                    $(current).prop('disabled', false);
                })
                .fail(function(xhr, status, error){
                    alert("Something Went Wrong! Status: " + xhr.responseText);
                    $(current).prop('checked', !current.checked);
                    $(current).prop('disabled', false);
                });
            changeDate(current);
        });

        // Handles checkbox changes
        $(".checkbox").change(function () {
            var current = this;
            data.checked = current.checked;
            data.service = current.id;
	        $(current).prop('disabled', true);
            $.post(updateUrl, data)
                .done(function(data, status){
                    $(current).prop('disabled', false);
                })
                .fail(function(xhr, status, error){
                    alert("Something Went Wrong! Status: " + xhr.responseText);
                    $(current).prop('checked', !current.checked);
                    $(current).prop('disabled', false);
                });
            changeDate(current);
        });

        // Handles install button presses
        $(".install").click(function () {
            alert("Installing Service...");
            var current = this;
            data.service = $(current).prev().find(".checkbox").attr('id');
            $.post(installUrl, data)
                .done(function(data, status){
                    alert("Installed Correctly!")
                    location.reload();
                })
                .fail(function(xhr, status, error){
                    alert("Something Went Wrong! Status: " + xhr.responseText);
                    location.reload();
                });
        });

        // Changes and formats "Last updated at" dates
        function changeDate(current) {
            var dt = new Date();
            var time = fix(dt.getHours()) + ":" + fix(dt.getMinutes()) + ":" + fix(dt.getSeconds()) + " on " + 
                       fix(dt.getDate()) + "/" + fix(dt.getMonth() + 1) + "/" + dt.getFullYear();
	    $(current).parent().parent().parent().find(".text-muted").html("Last updated at " + time);
        }

        function fix(number) {
            if (number < 10) {
                return "0" + number;
            } else {
                return "" + number;
            }
        }
    }
})();
