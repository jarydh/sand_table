

// // executed on startup after setup() as a script
// void execute()
// {
//     File f = SPIFFS.open(FILE_NAME);
//     if (!f.size() > 0)
//     {
//         Serial.printf("❌\tFailed to open file '%s'\n", FILE_NAME);
//         return;
//     }

//     while (f.available())
//     {
//         String line = f.readStringUntil('\n');
//         double x, y;

//         if (sscanf(line.c_str(), "%lf,%lf", &x, &y) == 2)
//         {
//             assert(x * x + y * y <= 1);
//             go_to(x, y);
//             delay(200);
//         }
//         else
//         {
//             Serial.printf("Skipping invalid line: %s\n", line.c_str());
//         }
//     }
//     f.close();
// }

