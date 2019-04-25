from django.db import models
from django.utils import timezone
from django.contrib.auth import get_user_model
from django.urls import reverse
from django.core.validators import FileExtensionValidator
from PIL import Image

# The Post class is a model for creating genereic posts
# These posts must include a title and content, but
# not a video or video thumbnail
class Post(models.Model):
  title = models.CharField(max_length=100)
  content = models.TextField()
  date_posted = models.DateTimeField(default=timezone.now) # passing in function for timezone
  author = models.ForeignKey(get_user_model(), on_delete=models.CASCADE) # CASCADE will delete post if user is deleted

  # Optional video fields
  # blank means that it is not required on the form when posting
  # null allows the database to not have an entry (i.e. no default)
  video = models.FileField(blank=True, null=True, upload_to='videos/%Y/%m/%d/', verbose_name='Video (optional)',
    validators=[FileExtensionValidator(allowed_extensions=['mp4', 'ogb', 'webm'])]) 

  video_thumbnail = models.ImageField(blank=True, default='default_thumbnail.jpg',  verbose_name='Video Thumbnail (optional)',
    upload_to='thumbnails/%Y/%m/%d/')

  # For resizing thumnail
  def save(self, *args, **kwargs):
    # Use the original save first 
    super().save(*args, **kwargs)

    # Resize the thumbnail
    thumbnail = Image.open(self.video_thumbnail.path)
    output_size = (160, 90)
    thumbnail.thumbnail(output_size)
    thumbnail.save(self.video_thumbnail.path)

  # Use title as the string
  def __str__(self):
    return self.title

  def get_absolute_url(self):
    return reverse('post-detail', kwargs={'pk': self.pk})

# The comment model is for creating comments within the post model
class Comment(models.Model):
  post = models.ForeignKey(Post, on_delete=models.CASCADE)
  author = models.ForeignKey(get_user_model(), on_delete=models.CASCADE)
  comment = models.TextField(verbose_name=u"")
  date_posted = models.DateTimeField(default=timezone.now)

