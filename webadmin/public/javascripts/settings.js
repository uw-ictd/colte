$.fn.exists = function () {
    return this.length !== 0;
}

const positiveErrorMsg = '<small class="error"> Please enter a nonnegative number </small>'
const power2ErrorMsg = '<small class="error"> Please enter a nonnegative power of 2 </small>'
const addressErrorMessage = '<small class="error"> Please enter a valid IP address </small>'
const addressWithMaskErrorMessage = '<small class="error"> Please enter a valid IP address and subnet mask </small>'

$(document).ready(function() {
    $("#dns").change(function(event) {
        if (event.target.checked == false) {
            var confirmation = confirm("Are you sure you want to disable local DNS? All other web services will be disabled.");
            if (confirmation == true) {
                $("#extra-fields").removeClass("invisible");
            } else {
                event.preventDefault();
                $("#extra-fields").addClass("invisible");
                $("#dns").prop("checked", true);
            }
        } else {
            $("#extra-fields").addClass("invisible");
        } 
    });

    // Prevent users from submitting form by pressing 'enter'
    $(window).keydown(function(event){
        if(event.keyCode == 13) {
          event.preventDefault();
          return false;
        }
    });

    // Validate user input
    $("#validate-btn").click(function() {
        var lteSubnet = $("#lte-subnet");
        var enbInterfaceAddress = $("#enb-interface-address");
        var maxEnb = $("#max-enb");
        var maxUe = $("#max-ue");
        var maxDl = $("#max-dl");
        var maxUl = $("#max-ul");
        var dnsAddress = $("#dns-address");
        var dnssecAddress = $("#dnssec-address");

        var errors = 0;

        if (lteSubnet.exists()) {
            errors = addErrorMessage(lteSubnet, addressWithMaskErrorMessage, isAddressWithMask, errors);
        }

        if (enbInterfaceAddress.exists()) {
            errors = addErrorMessage(enbInterfaceAddress, addressWithMaskErrorMessage, isAddressWithMask, errors);
        }

        if (maxEnb.exists()) {
            errors = addErrorMessage(maxEnb, power2ErrorMsg, isPower2, errors);
        }

        if (maxUe.exists()) {
            errors = addErrorMessage(maxUe, power2ErrorMsg, isPower2, errors);
        }

        if (maxDl.exists()) {
            errors = addErrorMessage(maxDl, positiveErrorMsg, isNonegative, errors);
        }

        if (maxUl.exists()){
            errors = addErrorMessage(maxUl, positiveErrorMsg, isNonegative, errors);
        }

        if (dnsAddress.exists() && !$("#extra-fields.invisible").exists()) {
            errors = addErrorMessage(dnsAddress, addressErrorMessage, isAddress, errors);
        }

        if (dnssecAddress.exists() &&  !$("#extra-fields.invisible").exists()) {
            errors = addErrorMessage(dnssecAddress, addressErrorMessage, isAddress, errors);
        }

        if (errors === 0) {
            document.getElementById("submit").click();
        }
    });
});

var isNonegative = function(value) {
    return parseInt(value) >= 0;
}

var isAddressWithMask = function(value) {
    var address = value.split('/');
    return isAddress(address[0]) && address[1] <= 32 && address[1] >= 0;
}

var isAddress = function(value) {
    var address = value.split('.');
    
    for (let i = 0; i < address.length; i++) {
        if (address[i] < 0 || address[i] > 255) {
            return false;
        }
    }

    return address.length === 4;
}

var isPower2 = function(value) {
    return Math.log2(value) % 1 === 0;
}

var addErrorMessage = function(element, message, test, errors) {
    var parent = element.parent();
    var lastChild = parent.children().last();
    if (!test(element.val())) {
        errors++;
        if (!lastChild.is("small.error")) {
            parent.append(message);
        }
    } else {
        if (lastChild.is("small.error")) {
            lastChild.remove();
        }
    }
    return errors;
}