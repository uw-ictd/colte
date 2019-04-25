from django.contrib.auth.backends import ModelBackend
from .models import CustomUser
from ipware.ip import get_ip
from django.contrib.auth.hashers import PBKDF2PasswordHasher

# Backend for getting ip
class IpBackend(ModelBackend):
  def authenticate(self, request, username=None, password=None):
    ip = get_ip(request)

    # Database should be set up to only allow unique ips
    try:
      user = CustomUser.objects.get(user_ip=ip) # Will fail if multiple users have same ip
      if user:
        return user

    except CustomUser.DoesNotExist:
      return None