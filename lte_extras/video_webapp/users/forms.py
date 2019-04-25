# users/forms.py
from django import forms
from django.contrib.auth.forms import UserCreationForm, UserChangeForm, AuthenticationForm
from .models import CustomUser, Profile
from django.contrib.auth import get_user_model

class CustomUserCreationForm(forms.ModelForm):
  class Meta(UserCreationForm):
    model = CustomUser
    fields = ('username',)

class CustomUserChangeForm(forms.ModelForm):
  class Meta:
    model = CustomUser
    fields = ('username',)

class ProfileUpdateForm(forms.ModelForm):
  class Meta:
    model = Profile
    fields = ('image',)

class CustomAuthenticationForm(AuthenticationForm):
  def confirm_login_allowed(self, user):
    if not user.is_active or not user.is_validated:
      raise forms.ValidationError('There was a problem with your login.', code='invalid_login')
