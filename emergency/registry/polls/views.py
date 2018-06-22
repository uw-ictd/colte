# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.shortcuts import render, get_object_or_404

from django.core.paginator import Paginator, EmptyPage, PageNotAnInteger

from django.http import HttpResponseRedirect

from ipware import get_client_ip

from .models import Person
from .forms import AddPersonForm

from django.db.models.functions import Lower

# Create your views here.
# def index(request):
#     person_list = Person.objects.all()
#     page = request.GET.get('page', 1)

#     paginator = Paginator(person_list, 10)
#     try:
#         persons = paginator.page(page)
#     except PageNotAnInteger:
#         persons = paginator.page(1)
#     except EmptyPage:
#         persons = paginator.page(paginator.num_pages)

#     return render(request, 'person_list.html', { 'persons': persons })

def detail(request, person_id):
	person = get_object_or_404(Person, pk=person_id)
	return render(request, 'detail.html', {'person': person})

def register(request):
    if request.method=='POST':
        form=AddPersonForm(request.POST)
        if form.is_valid():
            np = Person()
            np.name_text = form.cleaned_data['name_box']
            np.phone_number = form.cleaned_data['phone_box']
            np.location = form.cleaned_data['loc_box']
            # np.date = datetime.now()
            np.status = 1
            np.save()
            return HttpResponseRedirect('/')

    form = AddPersonForm()
    return render(request, 'polls/register.html', {'form': form})


def index(request):
    query = request.GET.get('search_box', None)

    number = request.GET.get('search_number', None)

    # Step 1: get client's IP (maybe translate to IMSI?!?)
    ip, is_routable = get_client_ip(request)
    if ip is None:
        # Unable to get the client's IP address - what's going on here?!?
        user = None
    else:
    # We got the client's IP address
        # Step 1: lookup user in table by IP
        user = None
        # if is_routable:
        # The client's IP address is publicly routable on the Internet
        # else:
        # The client's IP address is private

# Order of precedence is (Public, Private, Loopback, None)

    if query:
        if number:
            person_list = Person.objects.filter(phone_number__icontains=query).order_by(Lower('name_text'))
        else:
            person_list = Person.objects.filter(name_text__icontains=query).order_by(Lower('name_text'))
    else:
        person_list = Person.objects.all().order_by(Lower('name_text'))

    page = request.GET.get('page', 1)

    paginator = Paginator(person_list, 10)
    try:
        persons = paginator.page(page)
    except PageNotAnInteger:
        persons = paginator.page(1)
    except EmptyPage:
        persons = paginator.page(paginator.num_pages)

    return render(request, 'polls/person_list.html', { 'persons': persons, 'query': query, 'user': user })
