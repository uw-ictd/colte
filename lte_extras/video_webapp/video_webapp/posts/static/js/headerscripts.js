/* This is a collection of scripts that are loaded in the header
 * All new scripts should be written here
 */


/*
  Handles display toggling
  'Button' is the id of the html element
*/
function hide(button){
  var x = document.getElementById(button);
  if (x) {
    x.style.display = "none";
  }
}

function show(button) {
  var x = document.getElementById(button);
  if (x) {
    x.style.display = "block";
  }
}

function hideToggle(button) {
  var x = document.getElementById(button);
  if (x) {
    if (x.style.display === "none") {
      show(button)
    } else {
      hide(button)
    }
  }
}

function hideToggleDual(button1, button2) {
  hideToggle(button1)
  hideToggle(button2)
}

/*
  Handles AJAX calls for:
  - Form submission
  - Pagination
*/
$(function(){
  /****** AJAX forms ******/

  // Catches a form submit for a new comment
  $(document).on("submit", ".create-comment-form", function(event) {
    console.log("Create Form Submitted")

    // Prevents the submission from going through
    // Stops page reload
    event.preventDefault()

    // Hides the new comment form after submitting
    hideToggleDual('newcomment-toggle-button1','newcomment-toggle-button2')

    // Appends the form type to the data
    // Shows up in the request.POST dictionary
    data = $(this).serialize() + '&' + 'create' + '=' + $(this).attr('value')

    // Sends data via an ajax call
    ajaxFormSubmit(data)
  })

  // Catches a form submit for updating a comment
  $(document).on("submit", ".update-comment-form", function(event) {
    console.log("Update Form Submitted")
    event.preventDefault()
    data = $(this).serialize() + '&' + 'update' + '=' + $(this).attr('value')
    ajaxFormSubmit(data)
  })

  // Catches a form submit for deleting a comment
  $(document).on("submit", ".delete-comment-form", function(event) {
    console.log("Delete Form Submitted")
    event.preventDefault()
    data = $(this).serialize() + '&' + 'delete' + '=' + $(this).attr('value')
    ajaxFormSubmit(data)
  })

  // Sumbits the provided data
  function ajaxFormSubmit(data) {
    $.ajax({
      method: "POST",
      url: window.location.href,
      data: data,
      success: function(){
        reloadComments()
      },
      error: function(error){
        console.log(error);
        console.log("comment list error");
      }
    })
  }

  // Reloads the comment section only
  function reloadComments() {
    $.ajax({
      method: 'GET',
      url: window.location.href,
      dataType : 'json',
      data: {},
      success: function(data){
        console.log("Reload success")
        // Replace cpomment section with new data
        $('#comment-list').replaceWith($('#comment-list',data['content']));
      },
      error: function(error){
        console.log(error);
        console.log("comment list error");
      }
    });
  }

  /****** AJAX pagination ******/

  // Detects button presses in pagination section
  // Catches and sends the requested page number
  $(document).on("click", '.comment-pagination .pag-button', function(event) {
    event.preventDefault();
    url = ($('.comment-pagination .pag-button')[0].href);
    ajaxPagination($(this).attr('value'));
  });

  // Handles a pagination request for the provided page
  function ajaxPagination(page_num){
    console.log("Pagination pressed")
    // Very similar to reloadComments, but passes in page_num argument
    $.ajax({
      method: 'GET',
      url: window.location.href,
      dataType : 'json',
      data: {
        page : page_num
      },
      success: function(data){
        console.log("Pagination success")
        $('#comment-list').replaceWith($('#comment-list',data['content']));
      },
      error: function(error){
        console.log(error);
        console.log("comment list error");
      }
    });
  } 
})
