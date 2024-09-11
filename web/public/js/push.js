const rtf = new Intl.RelativeTimeFormat('en', { style: 'short' });

self.addEventListener('push', e => {
    const { public_code, last_push, last_seen, battery_low } = e.data?.json()?? {};

    if (battery_low !== undefined) {
        e.waitUntil(self.registration.showNotification(
            "Battery Low – Smart Mailbox",
            {
                body: public_code
                    ? `Your Smart Mailbox ${public_code} is low on battery power.`
                    : 'One of your Smart Mailboxes is low on battery power.',
                badge: '/res/favicon.svg',
                icon: '/res/favicon.svg',
                renotify: true,
                tag: `${public_code}_BATTERY_LOW`,
                timestamp: Date.parse(last_seen? last_seen: new Date())
            }
        ));

    } else if (last_push !== undefined) {
        e.waitUntil(self.registration.showNotification(
            "Mail has arrived – Smart Mailbox",
            {
                body: public_code
                    ? `Mail has just arrived to your Smart Mailbox ${public_code}.`
                    : 'Mail has just arrived to one of your Smart Mailboxes.',
                badge: '/res/favicon.svg',
                icon: '/res/favicon.svg',
                renotify: true,
                tag: public_code,
                timestamp: Date.parse(last_push? last_push: new Date())
            }
        ));
    }
});

self.addEventListener('notificationclick', e => {
    e.notification.close();
});
