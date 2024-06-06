// lib/viewmodels/monitoring_provider.dart
// ignore_for_file: prefer_typing_uninitialized_variables
import 'dart:async';
import 'dart:convert';
import 'dart:developer';
import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;

class MonitorProvider with ChangeNotifier {
  var suhuAirMonitor;
  var nutrisiMonitor;
  var intensitasCahayaMonitor;

  Future<void> fetchSuhuAir() async {
    final response = await http.get(Uri.parse(
        'https://smart-hydro-app-2f0c8-default-rtdb.asia-southeast1.firebasedatabase.app/.json'));
    if (response.statusCode == 200) {
      final Map<String, dynamic> data = jsonDecode(response.body);
      suhuAirMonitor = data['suhu_air']['Celcius'];
      notifyListeners();
    } else {
      log('Failed to load data: ${response.statusCode}');
    }
  }

  Future<void> fetchIntensitasCahaya() async {
    final response = await http.get(Uri.parse(
        'https://smart-hydro-app-2f0c8-default-rtdb.asia-southeast1.firebasedatabase.app/.json'));
    if (response.statusCode == 200) {
      final Map<String, dynamic> data = jsonDecode(response.body);
      intensitasCahayaMonitor = data['Cahaya']['Intensitas'];
      notifyListeners();
    } else {
      log('Failed to load data: ${response.statusCode}');
    }
  }

  Future<void> fetchNutrisi() async {
    final response = await http.get(Uri.parse(
        'https://smart-hydro-app-2f0c8-default-rtdb.asia-southeast1.firebasedatabase.app/.json'));
    if (response.statusCode == 200) {
      final Map<String, dynamic> data = jsonDecode(response.body);
      nutrisiMonitor = data['Nutrisi']['sensor'];
      notifyListeners();
    } else {
      log('Failed to load data: ${response.statusCode}');
    }
  }
}
