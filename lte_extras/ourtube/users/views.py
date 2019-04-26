from django.shortcuts import render, redirect
from django.contrib import messages
from django.contrib.auth import update_session_auth_hash, authenticate, login
from django.contrib.auth.decorators import login_required
from .backend import IpBackend
from .forms import CustomUserCreationForm, CustomUserChangeForm, ProfileUpdateForm, CustomAuthenticationForm
from .models import CustomUser
from ipware.ip import get_ip

def register(request):
  # First, check if the ip already has a user
  user = authenticate(request)

  # If user exists, log in and redirect to home
  if user:
    login(request, user)
    return redirect('home')

  # If user doesn't exist, continue with registration
  if request.method == 'POST':
    form = CustomUserCreationForm(request.POST)
    if form.is_valid():
      instance = form.save(commit=False)
      instance.user_ip = get_ip(request)
      instance.password = instance.user_ip
      if instance.user_ip:
        instance.save()
        username = form.cleaned_data.get('username')
        messages.success(request, f'Account created for {username}!')
         
        # With how login works, this is essentially a manual login after creation
        return redirect('login')

  else:
    form = CustomUserCreationForm()
  return render(request, 'users/register.html', {'form': form})
  
# This is a custom method to log in based on ip
def ip_login(request):
  # See if the request ip has an associated user
  user = authenticate(request)

  # User exists, log in and redirect to home
  if user:
    login(request, user)
    return redirect('home')

  # User does not exist, go to registration
  else:
    return redirect('register')

# Handles all profile updates
@login_required
def profile(request):
  # If the profile is being updated
  if request.method == 'POST':
    u_form = CustomUserChangeForm(request.POST, instance=request.user)
    p_form = ProfileUpdateForm(request.POST, 
                               request.FILES, 
                               instance=request.user.profile)
    
    # If the forms are valid
    if u_form.is_valid() and p_form.is_valid():
      u_form.save()
      p_form.save()
      messages.success(request, f'Your account has been updated!')
      return redirect('profile')

  # Fill in user info on profile page
  else :
    u_form = CustomUserChangeForm(instance=request.user)
    p_form = ProfileUpdateForm(instance=request.user.profile)
 
  context = {
    'u_form': u_form,
    'p_form': p_form
  }
  return render(request, 'users/profile.html', context)