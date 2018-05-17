# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models

# Create your models here.
class Person(models.Model):
	name_text = models.CharField(max_length=50)
	status = models.IntegerField(default=0)
	phone_number = models.CharField(max_length=15)
	location = models.CharField(max_length=200)
	def __str__(self):
		return self.name_text
