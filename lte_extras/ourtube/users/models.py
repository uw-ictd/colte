from django.db import models
from django.contrib.auth.models import AbstractUser
from django.contrib.auth import get_user_model
from PIL import Image

# Custom user profile that only has an ip
# password is set to ip
class CustomUser(AbstractUser):
  # add additional fields in here for custom user
  user_ip = models.GenericIPAddressField(blank=True, null=True, unique=True)
  password = models.GenericIPAddressField(blank=True, null=True)
  
# Profile is a model for attaching things like images to a user
class Profile(models.Model):
  user = models.OneToOneField(get_user_model(), on_delete=models.CASCADE) # Cascade removes profile if user is deleted
  image = models.ImageField(default='default.jpg', upload_to='profile_pics')

  def __str__(self):
    return f'{self.user.username} Profile'

  def save(self, *args, **kwargs): 
    super().save(*args, **kwargs)

    img = Image.open(self.image.path)
    
    # Resizes image
    if img.height > 300 or img.width > 300:
      output_size = (300, 300)
      img.thumbnail(output_size)
      img.save(self.image.path)