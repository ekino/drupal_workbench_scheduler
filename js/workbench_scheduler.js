/**
 * @file
 *
 * The javascript functionality for workbench Scheduler.
 * Sets the summary for Workbench Scheduler on vertical tabs.
 */

(function ($) {
    // Select all table checkbox.
    Drupal.behaviors.workbenchSchedulerSettingsSelectAllToggle = {
       attach: function (context) {
           // Trigger change event on all checkboxes.
           $('table.workbench-schedule-sid th.select-all > input.form-checkbox').change(function() {
               $('input.workbench-schedule-sid').change();
           });
       }
    }
    // Vertical tabs.
    Drupal.behaviors.workbenchSchedulerSettingsSummary = {
        attach: function (context) {

            $('#edit-workbench-scheduler', context).drupalSetSummary(function (context) {
                // Create summary array.
                var summary = [];

                // Loop through set schedules.
                var schedules = Drupal.settings.workbench_scheduler.schedules;
                for (var index in schedules) {
                    if (schedules.hasOwnProperty(index)) {
                        var schedule_type = schedules[index];
                        // Date and Time.
                        var date = $('input[name="workbench_scheduler_date['+schedule_type.sid+'][date]"]');
                        var time = $('input[name="workbench_scheduler_date['+schedule_type.sid+'][time]"]');

                        // Reformat date to be easier to understand
                        if (date.val() || time.val()) {
                            summary.push({
                            'sid': schedule_type.sid,
                            'label': schedule_type.label,
                            'date': date.val(),
                            'time': time.val()
                        });

                        }
                    }
                }

                // Remove any schedules that are unchecked.
                $('.workbench-schedule-sid input[type="checkbox"]:not(:checked)').each(function(){
                    var type_input = $(this);
                    for (index in summary) {
                        if (summary[index].sid == type_input.val()) {
                            delete summary[index]
                        }
                    }
                });

                // Compile final summary.
                var vals = [];
                for (var index in summary) {
                    if (summary.hasOwnProperty(index)) {
                        var schedule = summary[index];
                        vals.push(schedule.label + ': ' + Drupal.t('@date @time', {'@date':schedule.date, '@time':schedule.time}));
                    }
                }
                return vals.join("<br />");
            });
        }
    };
})(jQuery);
