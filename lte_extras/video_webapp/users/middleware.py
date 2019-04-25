from django.shortcuts import redirect, reverse
from django.conf import settings
from posts.models import Post
from django.urls import resolve
import datetime, os

# Middleware that makes the user always log in
def AuthRequiredMiddleware(get_response):
  # Can put one time initializations here

  # middleware for requiring login
  def middleware(request):
    response = get_response(request)

    # Allow user to continue if logged in
    # Does not redirect if on the admin, login, or register page without being logged in
    if request.path.startswith('/admin/logout'):
      return redirect('admin:login')
    elif request.path.startswith('/admin/'):
      return response
    elif request.user.is_authenticated or request.path == reverse('login') or request.path.startswith('/favicon.ico'):
      return response
    elif not request.user.is_authenticated and request.path == reverse('register'):
      return response
    else:
      return redirect('login')

  return middleware

# Middleware that logs user activity
def LoggingMiddleware(get_response):

  def middleware(request):
    response = get_response(request)

    try:
      current_url = resolve(request.path_info).url_name

      if request.user.is_authenticated:
        # Get the log directory
        LOG_DIRECTORY = settings.LOGGING_ROOT + '/user_activity.log'
        log_data = {}

        # Record info
        log_data["user_ip"]    = str(request.user.user_ip)
        log_data["path"]       = str(request.path)
        log_data["time_stamp"] = str(datetime.datetime.now())

        # User is accessing a post
        if current_url == 'post-detail':

          # Get the post pk from substring of /posts/<int:pk>
          post_pk = int(str(request.path)[7:-1])
          log_data["post_id"] = post_pk
          log_data["post_name"] = str(Post.objects.get(pk=post_pk))

          # User viewed post
          if request.method == 'GET':
            log_data["action"] = 'viewed post'
          
          # User action with a comment
          elif request.method == 'POST':
            if 'create' in request.POST:
              log_data["action"] = 'created comment'
            if 'update' in request.POST:
              log_data["action"] = 'updated comment'
            if 'delete' in request.POST:
              log_data["action"] = 'deleted comment'

        # User submitted something
        elif request.method == 'POST':    
          if current_url == 'post-create':
            log_data["action"] = 'created post'
          elif current_url == 'post-update':
            log_data["action"] = 'updated post'
          elif current_url == 'post-delete':
            log_data["action"] = 'deleted post'
          elif current_url == 'profile':
            log_data["action"] = 'updated profile'

        # No useful data, clear log
        else:
          log_data = {}

        # Log any data that was collected
        if len(log_data) > 0:
          # Write to log file
          log_file = open(LOG_DIRECTORY, 'a')
          log_file.write(str(log_data) + "\n")
          log_file.close()

    except Exception as e:
      print("Failed to log: " + str(e))

    finally:
      return response

  return middleware